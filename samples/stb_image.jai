// extern stbi_uc *stbi_png_load             (char const *filename,     int *x, int *y, int *comp, int req_comp);
// extern stbi_uc *stbi_png_load_from_file   (FILE *f,                  int *x, int *y, int *comp, int req_comp);

stbi_png_load :: (filename: *u8, x: * s32, y: * s32, comp: * s32, req_comp: s32) -> * u8  #foreign;

