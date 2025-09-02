#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <cmath>

enum BIOME
{
    DESERT,
    PLAINS,
    FOREST,
    MOUNTAINS
};

#define DESERT_BASE 50
#define PLAINS_BASE 58
#define FOREST_BASE 64
#define MOUNTAINS_BASE 90

#define DESERT_AMP 6
#define PLAINS_AMP 10
#define FOREST_AMP 14
#define MOUNTAINS_AMP 40

#define DESERT_ROUGH 0.40
#define PLAINS_ROUGH 0.45
#define FOREST_ROUGH 0.50
#define MOUNTAINS_ROUGH 0.65

#define TM_PERIOD 100.0f

#define DESERT_CENTERX 0.85
#define DESERT_CENTERY 0.20

#define PLAINS_CENTERX 0.55
#define PLAINS_CENTERY 0.50

#define FOREST_CENTERX 0.50
#define FOREST_CENTERY 0.80

#define MOUNTAINS_CENTERX 0.55f
#define MOUNTAINS_CENTERY 0.85f

#define NUM_BIOMES 4
struct BIOME_DISTRIBUTION
{
    float desert;
    float plains;
    float forest;
    float mountains;
};

class Biome
{
public:
    static int base[NUM_BIOMES];

    static BIOME_DISTRIBUTION get_biome_distribution(glm::vec2 xz);

    static int get_height(glm::vec2 xz);

    static BIOME get_biome(glm::vec2 xz);
};