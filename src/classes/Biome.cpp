#include "../../include/Biome.hpp"

static int base[4] = {DESERT_BASE, PLAINS_BASE, FOREST_BASE, MOUNTAINS_BASE};

BIOME_DISTRIBUTION Biome::get_biome_distribution(glm::vec2 xz)
{
    // float T = glm::perlin(xz / TM_PERIOD);
    // T = 0.5f * (T + 1.0f);

    // glm::vec2 xzM = (xz + glm::vec2(1000.0f, -1000.0f)) / TM_PERIOD;
    // float M = 0.5f * (glm::perlin(xzM) + 1.0f);

    glm::vec2 q(glm::perlin(xz / 900.0f + glm::vec2(5.2f, 1.3f)),
                glm::perlin(xz / 900.0f + glm::vec2(-3.7f, 2.1f)));
    glm::vec2 xzB = xz + 40.0f * q; // small bend of the climate space

    float T = 0.5f * (glm::perlin(xzB / TM_PERIOD) + 1.0f);
    float M = 0.5f * (glm::perlin((xzB + glm::vec2(1000, -1000)) / TM_PERIOD) + 1.0f);

    // small jitter still ok:
    T += 0.05f * glm::perlin(xz / 150.0f);
    M += 0.05f * glm::perlin(xz / 180.0f + 100.0f);
    T = glm::clamp(T, 0.0f, 1.0f);
    M = glm::clamp(M, 0.0f, 1.0f);

    auto gaussian = [](float d, float sigma)
    {
        return std::exp(-(d * d) / (2.0f * sigma * sigma));
    };

    glm::vec2 desertC(DESERT_CENTERX, DESERT_CENTERY);
    glm::vec2 plainsC(PLAINS_CENTERX, PLAINS_CENTERY);
    glm::vec2 forestC(FOREST_CENTERX, FOREST_CENTERY);

    float sigma = 0.20f;

    // auto T2 = T + 0.05 * perlin(xz / 150.0f); // small amplitude
    // auto M2 = M + 0.05 * perlin(xz / 180.0f + 100.0f);
    glm::vec2 tm(T, M);

    float sDesert = gaussian(glm::distance(tm, desertC), sigma);

    float sPlains = gaussian(glm::distance(tm, plainsC), sigma);

    float sForest = gaussian(glm::distance(tm, forestC), sigma);

    // float E = 0.5f * (glm::perlin(xz / 1000.0f) + 1.0f);
    float E = 0.5f * (glm::perlin(xz / 1200.0f) + 1.0f); // mountain mask scale
    float mLow = glm::smoothstep(0.45f, 0.60f, E);       // foothills
    float mHigh = glm::smoothstep(0.70f, 0.85f, E);      // peaks
    float mFoot = glm::max(mLow - mHigh, 0.0f);

    // penalize plains where foothills present; slightly boost forest there
    sPlains *= (1.0f - 0.7f * mFoot);
    sForest *= (1.0f + 0.3f * mFoot);
    float sMtn = glm::smoothstep(MOUNTAINS_CENTERX, MOUNTAINS_CENTERY, E);

    sDesert *= 1.30f;
    sForest *= 1.20f;
    sPlains *= 0.90f;
    sMtn *= 0.90f;

    float sum = sDesert + sPlains + sForest + sMtn + 1e-6f;
    float wDesert = sDesert / sum;
    float wPlains = sPlains / sum;
    float wForest = sForest / sum;
    float wMtn = sMtn / sum;

    return {wDesert, wPlains, wForest, wMtn};
}

int Biome::get_height(glm::vec2 xz)
{
    auto biome_distrib = get_biome_distribution(xz);

    float base = 0, amp_biome = 0, rough = 0;

    base += biome_distrib.desert * DESERT_BASE;
    base += biome_distrib.plains * PLAINS_BASE;
    base += biome_distrib.forest * FOREST_BASE;
    base += biome_distrib.mountains * MOUNTAINS_BASE;

    amp_biome += biome_distrib.desert * DESERT_AMP;
    amp_biome += biome_distrib.plains * PLAINS_AMP;
    amp_biome += biome_distrib.forest * FOREST_AMP;
    amp_biome += biome_distrib.mountains * MOUNTAINS_AMP;

    rough += biome_distrib.desert * DESERT_ROUGH;
    rough += biome_distrib.plains * PLAINS_ROUGH;
    rough += biome_distrib.forest * FOREST_ROUGH;
    rough += biome_distrib.mountains * MOUNTAINS_ROUGH;

    // Macro uplift
    float macro = glm::perlin(xz / 3000.0f); // [-1,1]

    // fBm detail (octaves): use separate variables
    float lacunarity = 2.0f;
    float gain = glm::mix(0.45f, 0.60f, glm::clamp((rough - 0.40f) / (0.65f - 0.40f), 0.0f, 1.0f));

    float detail = 0.0f;
    float layer_amp = 1.0f;     // internal octave amplitude
    float freq = 1.0f / 120.0f; // base frequency (feature size 120 blocks)

    const int OCT = 4; // 4 layers
    for (int i = 0; i < OCT; ++i)
    {
        detail += layer_amp * glm::perlin(xz * freq); // perlin in [-1,1]
        freq *= lacunarity;                           // features 2x smaller each octave
        layer_amp *= gain;                            // each octave contributes less
    }

    // Normalize fBm to roughly [-1,1]
    // max_sum ~= 1 + gain + gain^2 + ... + gain^(OCT-1)
    float max_sum = (1.0f - std::pow(gain, OCT)) / (1.0f - gain);
    detail /= max_sum;

    float h = base + amp_biome * detail + 30.0f * macro;
    return (int)std::floor(h);
}

inline float weight_of(const BIOME_DISTRIBUTION &w, BIOME b)
{
    switch (b)
    {
    case DESERT:
        return w.desert;
    case PLAINS:
        return w.plains;
    case FOREST:
        return w.forest;
    case MOUNTAINS:
        return w.mountains;
    }
    return 0.0f;
}

BIOME Biome::get_biome(glm::vec2 xz)
{

    auto biome_distrib = get_biome_distribution(xz);
    float curr_max = 0;
    BIOME curr_biome = DESERT;
    for (int i = 0; i < NUM_BIOMES; i++)
    {
        if (weight_of(biome_distrib, (BIOME)i) >= curr_max)
        {
            curr_max = weight_of(biome_distrib, (BIOME)i);
            curr_biome = (BIOME)i;
        }
    }

    return curr_biome;
}