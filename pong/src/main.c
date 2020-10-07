#include <stdio.h>
#include "../lib/lua/src/lua.h"
#include "../lib/lua/src/lualib.h"
#include "../lib/lua/src/lauxlib.h"


void luaC_dofile(void)
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    luaL_dofile(L, "./scripts/logic.lua");
    lua_close(L);
}

void luaC_getvar(void)
{
    lua_State* L = luaL_newstate();

    luaL_dostring(L, "some_var = 486");
    lua_getglobal(L, "some_var");
    lua_Number some_c_var = lua_tonumber(L, -1);
    printf("The value of some_var inside C is: %d\n", (int)some_c_var);

    lua_close(L);
}

void luaC_stack(void)
{
    lua_State *L = luaL_newstate();

    lua_pushnumber(L, 286);
    lua_pushnumber(L, 386);
    lua_pushnumber(L, 486);

    lua_Number element;
    element = lua_tonumber(L, -1);
    printf("The last added element in position 3 of the stack is %d\n", (int)element);

    lua_remove(L, 2);
    element = lua_tonumber(L, 2);
    printf("Position 2 of the stack is now %d\n", (int)element);

    lua_close(L);
}

void luaC_call_lua_function(void)
{
    lua_State* L = luaL_newstate();

    if (luaL_dofile(L, "./scripts/logic.lua") != LUA_OK) 
    {
        luaL_error(L, "Error: %s\n",lua_tostring(L, -1));
    }

    lua_getglobal(L, "logic");

    if (lua_isfunction(L, -1))
    {
        lua_pushnumber(L, 3);
        lua_pushnumber(L, 4);
        const int NUM_ARGS = 2;
        const int NUM_RETURNS = 1;

        if (lua_pcall(L, NUM_ARGS, NUM_RETURNS, 0))
        {
            //TODO: error...
        }
        else
        {
            lua_Number result = lua_tonumber(L, -1);
            printf("the factorial of 3 is: %f\n", (float)result);
        }
    }

    lua_close(L);
}

int native_logic(lua_State* L)
{
    lua_Number b = lua_tonumber(L, -1);
    lua_Number a = lua_tonumber(L, -2);
    lua_Number result = (a * a) + (b * b);
    lua_pushnumber(L, result);

    return 1;
}

void luaC_call_C_function(void)
{
    lua_State* L = luaL_newstate();

    lua_pushcfunction(L, native_logic);
    lua_setglobal(L, "native_logic");
    
    luaL_dofile(L, "./scripts/logic.lua");

    lua_getglobal(L, "logic");
    if (lua_isfunction(L, -1))
    {
        lua_pushnumber(L, 3);   // send values for native_logic
        lua_pushnumber(L, 4);

        const int NUM_ARGS = 2;
        const int NUM_RETURNS = 1;
    
        lua_pcall(L, NUM_ARGS, NUM_RETURNS, 0);
        lua_Number logic_result = lua_tonumber(L, -1);
        printf("Native logic(3, 4) = %f\n", (float)logic_result);
    }

    lua_close(L);
}

typedef struct rectangle2d {
    int x;
    int y;
    int width;
    int height;
} rectangle;

int create_rectangle(lua_State* L)
{
    rectangle* rect = (rectangle*)lua_newuserdata(L, sizeof(rectangle));
    rect->x = 0;
    rect->y = 0;
    rect->width = 0;
    rect->height = 0;

    return 1;
}

int change_rectangle_size(lua_State* L) 
{
    rectangle* rect = (rectangle*)lua_touserdata(L, 1);
    rect->width = (int)lua_tonumber(L, 2);
    rect->height = (int)lua_tonumber(L, 3);
    return 0;
}

void lua_userdata(void) 
{
    lua_State* L = luaL_newstate();
    
    // expose native create_rectangle function to lua
    lua_pushcfunction(L, create_rectangle);
    lua_setglobal(L, "create_rectangle");

    // expose the native change_rectangel_size function to lua
    lua_pushcfunction(L, change_rectangle_size);
    lua_setglobal(L, "change_rectangle_size");

    luaL_dofile(L, "./scripts/rectangle.lua");    
    


    lua_getglobal(L, "square");
    if (lua_isuserdata(L, -1))
    {
        rectangle* r = lua_touserdata(L, -1);
        printf("Rectangle width: %d, height: %d\n", r->width, r->height);
    }
    else
    {
        printf("Did not get rectangle userdata from lua.");
    }

    lua_close(L);
}


int main(int argc, char *argv[])
{
    //luaC_dofile();
    //luaC_getvar();
    //luaC_stack();
    //luaC_call_lua_function();
    //luaC_call_C_function();
    lua_userdata();

    return (1);
}
