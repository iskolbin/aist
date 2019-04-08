#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#include <stb/stb_image.h>


static void aist_register_metatable(lua_State *L, const char *mt_name, const luaL_Reg mt_functions[]) {
	luaL_newmetatable(L, mt_name);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
#if LUA_VERSION_NUM <= 501
	luaL_register(L, NULL, mt_functions);
#else
	luaL_setfuncs(L, mt_functions, 0);
#endif	
}


typedef struct Image {
	unsigned char *data;
	int width;
	int height;
	int componentsperpixel;
} Image;


static int aist_gfx_Image_get_width(lua_State *L) {
	Image *image = luaL_checkudata(L, 1, "aist_gfx_Image");
	lua_pushinteger(L, image->width);
	return 1;
}

static int aist_gfx_Image_get_height(lua_State *L) {
	Image *image = luaL_checkudata(L, 1, "aist_gfx_Image");
	lua_pushinteger(L, image->height);
	return 1;
}

static int aist_gfx_Image_get_componentsperpixel(lua_State *L) {
	Image *image = luaL_checkudata(L, 1, "aist_gfx_Image");
	lua_pushinteger(L, image->componentsperpixel);
	return 1;
}

int aist_gfx_Image_release(lua_State *L) {
	Image *image = luaL_checkudata(L, 1, "aist_gfx_Image");
	if (image->data != NULL) {
		free(image->data);
		image->data = NULL;
		image->width = 0;
		image->height = 0;
		image->componentsperpixel = 0;
	}
	return 0;
}

static const luaL_Reg aist_gfx_Image_metatable[] = {
	{"__gc", aist_gfx_Image_release},
	{"getwidth", aist_gfx_Image_get_width},
	{"getheight", aist_gfx_Image_get_height},
	{"getcomponentsperpixel", aist_gfx_Image_get_componentsperpixel},
	{NULL, NULL}
};

int aist_gfx_load_image(lua_State *L) {
	int width, height, componentsperpixel;
	unsigned char *data;
	const char *filename = luaL_checkstring(L, 1);
	if (filename == NULL) return luaL_error(L, "Cannot read image");
	data = stbi_load(filename, &width, &height, &componentsperpixel, 0);
	if (data == NULL) return luaL_error(L, "Cannot read image");
	Image *image = lua_newuserdata(L, sizeof *image);
	image->data = data;
	image->width = width;
	image->height = height;
	image->componentsperpixel = componentsperpixel;
	luaL_setmetatable(L, "aist_gfx_Image");
	return 1;
}

static const luaL_Reg aist_gfx[] = {
	{"loadimage", aist_gfx_load_image},
	{NULL, NULL}
};

static int aist_gfx_register(lua_State *L) {
	aist_register_metatable(L, "aist_gfx_Image", aist_gfx_Image_metatable);
	luaL_newlib(L, aist_gfx);
	return 1;
}

static const luaL_Reg aist[] = {
	{NULL, NULL}
};

LUAMOD_API int luaopen_aist(lua_State *L) {
	luaL_newlib(L, aist);
	aist_gfx_register(L); lua_setfield(L, -2, "gfx");
	return 1;
}
