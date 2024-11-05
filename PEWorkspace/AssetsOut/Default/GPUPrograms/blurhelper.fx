#ifndef HLSL_BLURHELPER
#define HLSL_BLURHELPER

#define bw0 0.14
#define bw1 0.17
#define bw2 0.20
#define bw3 0.23
#define bw4 0.25
#define bw5 0.27
#define bw6 0.29
#define bw7 0.27
#define bw8 0.25
#define bw9 0.23
#define bw10 0.20
#define bw11 0.17
#define bw12 0.14

#if !APIABSTRACTION_IOS // doesnt allow const arrays..
static const float BlurWeights[13] = {
bw0,
bw1,
bw2,
bw3,
bw4,
bw5,
bw6,
bw7,
bw8,
bw9,
bw10,
bw11,
bw12
};
#endif

#if APIABSTRACTION_IOS
#define MAP_W (768.0)
#define MAP_H (1024.0)

#elif APIABSTRACTION_PS3

#define MAP_W (1920.0)
#define MAP_H (1080.0)

#else

#define MAP_W (1280.0f)
#define MAP_H (720.0f)

#endif

#define GLOW_MAP_DIV_FACTOR (2.0)

#define MAP_DX (1.0 / (MAP_W / GLOW_MAP_DIV_FACTOR))

#define MAP_DY (1.0 / (MAP_H / GLOW_MAP_DIV_FACTOR))

#endif
