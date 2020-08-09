#if !defined(SCRIPTING_TYPES_H)
#define SCRIPTING_TYPES_H

template <typename Handler>
static bool sol_lua_check(sol::types<v2>, lua_State* L, i32 index, Handler&& handler, sol::stack::record& tracking) {
	i32 absoluteIndex = lua_absindex(L, index);
	
    if (!stack::check<sol::table>(L, absoluteIndex, handler)) {
        tracking.use(1);
        return false;
    }

    sol::stack::get_field(L, "x", absoluteIndex);
    bool x = sol::stack::check<float>(L, -1);
    sol::stack::get_field(L, "y", absoluteIndex);
    bool y = sol::stack::check<float>(L, -1);

    sol::stack::pop_n(L, 2);
    tracking.use(1);
    
	return x && y;
}

static v2 sol_lua_get(sol::types<v2>, lua_State* L, i32 index, sol::stack::record& tracking) {
	i32 absoluteIndex = lua_absindex(L, index);

    sol::table table = sol::stack::get<sol::table>(L, absoluteIndex);
    f32 x = table["x"];
    f32 y = table["y"];

	tracking.use(1);

	return V2(x, y);
}

static int sol_lua_push(sol::types<v2>, lua_State* L, const v2& vector2) {
    lua_createtable(L, 0, 2);

    lua_getglobal(L, "v2");
    lua_setmetatable(L, -2);
    
    sol::stack_table vec2(L);
    vec2["x"] = vector2.x;
    vec2["y"] = vector2.y;

	return 1;
}

#endif