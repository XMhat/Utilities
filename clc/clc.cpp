/* ------------------------------------------------------------------------- */
// Compile with: cls; g++ -O3 --std c++20 -Iinclude/lua clc.cpp lua.a -o clc
/* ------------------------------------------------------------------------- */
#include <exception>                   // std::exception
#include <functional>                  // std::function
#include <iomanip>                     // std::precision
#include <iostream>                    // std::cout
#include <memory>                      // std::unique_ptr
/* ------------------------------------------------------------------------- */
#include <lua.h>                       // Lua base
#include <lualib.h>                    // Lua api
#include <lauxlib.h>                   // Lua aux api
/* ------------------------------------------------------------------------- */
int main(int iArgC, char **cpaArgV) try
{ // If not enough argumemts?
  if(iArgC <= 1)
  { // Show usage
    std::cout << "Usage: "
              << *cpaArgV
              << " [expression]."
              << std::endl
              << "Compiled: " __TIMESTAMP__ "."
              << std::endl
              << "Lua version: " LUA_RELEASE "."
              << std::endl;
    // Return error staus
    return 1;
  } // Prototype for lua context self release state
  typedef std::unique_ptr<lua_State,
    std::function<decltype(lua_close)>> LuaPtr;
  // Make STL ask LUA to create a new state and if successful?
  if(const LuaPtr lpState{
    // Call back memory allocation function
    lua_newstate([](void*const, void*const vpPtr, size_t, size_t stSize)->void*
    { // (Re)allocate if memory needed and return
      if(stSize) return realloc(vpPtr, stSize);
      // Zero for free memory
      free(vpPtr);
      // Return nothing
      return nullptr;
    }, nullptr), lua_close }) try
  { // Get address of LUA state data
    lua_State*const lsState = lpState.get();
    // Set panic callback that throws and error
    lua_atpanic(lsState, [](lua_State*const lsState)->int
      { throw std::runtime_error{ lua_tostring(lsState, 1) }; });
    // Build mathemetics api function table
    luaopen_math(lsState);
    // Assign it to the global index
    lua_rawseti(lsState, LUA_REGISTRYINDEX, LUA_RIDX_GLOBALS);
    // Space character
    const std::string strSpace{ " " };
    // Concatenate string buffer from command line
    std::string strBuffer; strBuffer.reserve(1024);
    const int iArgCM1 = iArgC-1;
    for(int iArg = 1; iArg < iArgCM1; ++iArg)
      { strBuffer += cpaArgV[iArg]; strBuffer += strSpace; }
    strBuffer += cpaArgV[iArgCM1];
    // Compile the specified code snipped and capture result
    const std::string strSnippet{ "return (" + strBuffer + ");" };
    switch(const int iResult = luaL_loadbuffer(lsState, strSnippet.c_str(),
      strSnippet.length(), strBuffer.c_str()))
    { // No error? Break to execution
      case 0: break;
      // Syntax error?
      case LUA_ERRSYNTAX:
      { // Inform user
        std::cout << "Compile error! > "
                  << lua_tostring(lsState, 1)
                  << std::endl;
        // Exit failure
        return 2;
      } // Memory allocation error?
      case LUA_ERRMEM:
      { // Inform user
        std::cout << "Lua is out of memory error compiling!"
                  << std::endl;
        // Exit failure
        return 3;
      } // Unknown error?
      default:
      { // Inform user
        std::cout << "Unknown compile error "
                  << iResult
                  << '!' << std::endl;
        // Exit failure
        return 4;
      }
    } // Execute function with no parameters
    switch(const int iResult = lua_pcall(lsState, 0, 1, 0))
    { // No error so break to show result
      case 0: break;
      // Runtime error?
      case LUA_ERRRUN:
      { // Inform user
        std::cout << "Run-time error! > "
                  << lua_tostring(lsState, 1)
                  << std::endl;
        // Exit failure
        return 5;
      } // Memory allocation error
      case LUA_ERRMEM:
      { // Inform user
        std::cout << "Lua is out of memory error executing!" << std::endl;
        // Exit failure
        return 6;
      } // Error + error in error handler
      case LUA_ERRERR:
      { // Inform user
        std::cout << "Lua got error in error handler!" << std::endl;
        // Exit failure
        return 7;
      } // Unknown error?
      default:
      { // Inform user
        std::cout << "Unknown run-time error " << iResult << '!' << std::endl;
        // Exit failure
        return 8;
      }
    } // Begin printing result
    std::cout << "The result of '"
              << strBuffer
              << "' is ";
    // What type of LUA variable was returned?
    switch(const int iType = lua_type(lsState, 1))
    { // A number value?
      case LUA_TNUMBER:
      { // If not actually an integer?
        if(!lua_isinteger(lsState, 1))
        { // Write as normal floating-point number
          std::cout << "a number of "
                    << std::setprecision(12)
                    << std::fixed
                    << lua_tonumber(lsState, 1);
        } // Get actual integer?
        else
        { // Get the integer value and inform user
          const lua_Integer liValue = lua_tointeger(lsState, 1);
          std::cout << "an integer of "
                    << liValue
                    << std::hex
                    << " [0x"
                    << liValue
                    << ']';
          // Print unsigned value if needed
          if(liValue < 0)
            std::cout << std::endl
                      << "or an unsigned value of "
                      << std::dec
                      << static_cast<lua_Unsigned>(liValue);
        } // Done
        break;
      } // Who knows? Function? Userdata?
      default:
      { // Write as unknown type
        std::cout << "(type "
                  << iType
                  << ") not a number";
        // Done
        break;
      }
    } // Write rest of line
    std::cout << '!' << std::endl;
  } // Exception occured in sandbox?
  catch(const std::exception &e)
  { // Inform user
    std::cout << "Sandbox exception: " << e.what() << "!" << std::endl;
    // Exit failure
    return 9;
  } // Lua failed to create state?
  else
  { // Inform user
    std::cout << "Error creating lua state!" << std::endl;
    // Exit failure
    return 10;
  } // Exit success
  return 0;
} // Exception occured in root?
catch(const std::exception &e)
{ // Inform user
  std::cout << "Exception: " << e.what() << "!" << std::endl;
  // Exit failure
  return 11;
}
/* ------------------------------------------------------------------------- */
