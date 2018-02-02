@echo OFF

if exist client\lua\lua\bin\liblua.a (goto CHIMERA)

:LUA

cd client\lua\lua
echo Building Lua...

del bin /Q
for /r %%f in (*.c) do gcc -c %%f -O2 -o bin/%%~nf.o
ar rcs liblua.a bin/*
move liblua.a bin

echo Done building Lua

cd ..\..\..

:CHIMERA

if "%1" == "release" (goto RELEASE)

:DEBUG
SET ARGS=-g
SET ARGSFAST=-g
SET LARGS=-g
goto BUILD

:RELEASE
SET ARGS=-O2
SET ARGSFAST=-Ofast -msse3 -fno-exceptions
SET LARGS=-Wl,--exclude-all-symbols -s
goto BUILD

:BUILD

del bin /Q

@echo ON
windres version.rc -o bin/version.o

g++ -c main.cpp %ARGS% -o bin/main.o

g++ -c client/client_signature.cpp %ARGS% -o bin/client__client_signature.o
g++ -c client/client.cpp %ARGS% -o bin/client__client.o
g++ -c client/path.cpp %ARGS% -o bin/client__path.o
g++ -c client/settings.cpp %ARGS% -o bin/client__settings.o

g++ -c client/command/command.cpp %ARGS% -o bin/client__command__command.o
g++ -c client/command/console.cpp %ARGS% -o bin/client__command__console.o

g++ -c client/fix/descope_fix.cpp %ARGS% -o bin/client__fix__descope_fix.o
g++ -c client/fix/magnetism_fix.cpp %ARGS% -o bin/client__fix__magnetism_fix.o
g++ -c client/fix/widescreen_fix.cpp %ARGS% -o bin/client__fix__widescreen_fix.o

g++ -c client/debug/budget.cpp %ARGS% -o bin/client__debug__budget.o
g++ -c client/debug/wireframe.cpp %ARGS% -o bin/client__debug__wireframe.o

g++ -c client/enhancements/auto_center.cpp %ARGS% -o bin/client__enhancements__auto_center.o
g++ -c client/enhancements/firing_particle.cpp %ARGS% -o bin/client__enhancements__firing_particle.o
g++ -c client/enhancements/multitexture_overlay.cpp %ARGS% -o bin/client__enhancements__mo.o
g++ -c client/enhancements/show_spawn.cpp %ARGS% -o bin/client__enhancements__show_spawn.o
g++ -c client/enhancements/skip_loading.cpp %ARGS% -o bin/client__enhancements__skip_loading.o
g++ -c client/enhancements/throttle_fps.cpp %ARGS% -o bin/client__enhancements__throttle_fps.o
g++ -c client/enhancements/uncap_cinematic.cpp %ARGS% -o bin/client__enhancements__uncap_cinematic.o
g++ -c client/enhancements/zoom_blur.cpp %ARGS% -o bin/client__enhancements__zoom_blur.o

g++ -c client/hac2/hac2.cpp %ARGS% -o bin/client__hac2__hac2.o
g++ -c client/hac2/scope_fix.cpp %ARGS% -o bin/client__hac2__scope_fix.o

g++ -c client/halo_data/global.cpp %ARGS% -o bin/client__halo_data__global.o
g++ -c client/halo_data/map.cpp %ARGS% -o bin/client__halo_data__map.o
g++ -c client/halo_data/server.cpp %ARGS% -o bin/client__halo_data__server.o
g++ -c client/halo_data/spawn_object.cpp -masm=intel -o bin/client__halo_data__spawn_object.o
g++ -c client/halo_data/script.cpp -masm=intel -o bin/client__halo_data__script.o
g++ -c client/halo_data/table.cpp %ARGS% -o bin/client__halo_data__table.o
g++ -c client/halo_data/tag_data.cpp %ARGS% -o bin/client__halo_data__tag_data.o
g++ -c client/halo_data/tiarace/hce_tag_class_int.cpp %ARGS% -o bin/client__halo_data__tiarace__hce_tag_class_int.o

g++ -c client/hooks/camera.cpp %ARGS% -o bin/client__hooks__camera.o
g++ -c client/hooks/frame.cpp %ARGS% -o bin/client__hooks__frame.o
g++ -c client/hooks/map_load.cpp %ARGS% -o bin/client__hooks__map_load.o
g++ -c client/hooks/rcon_message.cpp %ARGS% -o bin/client__hooks__rcon_message.o
g++ -c client/hooks/tick.cpp %ARGS% -o bin/client__hooks__tick.o

g++ -c client/hud_mod/offset_hud_elements.cpp %ARGS% -o bin/client__hud_mod__offset_hud_elements.o

g++ -c client/interpolation/camera.cpp %ARGSFAST% -o bin/client__interpolation__camera.o
g++ -c client/interpolation/fp.cpp %ARGSFAST% -o bin/client__interpolation__fp.o
g++ -c client/interpolation/light.cpp %ARGSFAST% -o bin/client__interpolation__light.o
g++ -c client/interpolation/interpolation.cpp %ARGSFAST% -o bin/client__interpolation__interpolation.o
g++ -c client/interpolation/particle.cpp %ARGSFAST% -o bin/client__interpolation__particle.o
g++ -c client/interpolation/widget.cpp %ARGS% -o bin/client__interpolation__widget.o

g++ -c client/lua/lua.cpp %ARGS% -o bin/client__lua__lua.o
g++ -c client/lua/lua_callback.cpp %ARGS% -o bin/client__lua__lua_callback.o
g++ -c client/lua/lua_game.cpp %ARGS% -o bin/client__lua__lua_game.o
g++ -c client/lua/lua_io.cpp %ARGS% -o bin/client__lua__lua_io.o

g++ -c client/messaging/messaging.cpp -masm=intel -o bin/client__messaging_messaging.o

g++ -c client/visuals/anisotropic_filtering.cpp %ARGS% -o bin/client__visuals__af.o
g++ -c client/visuals/set_resolution.cpp -masm=intel -o bin/client__visuals__set_resolution.o
g++ -c client/visuals/vertical_field_of_view.cpp %ARGS% -o bin/client__visuals__vertical_fov.o

g++ -c client/xbox/safe_zone.cpp %ARGS% -o bin/client__xbox__safe_zone.o

g++ -c code_injection/codefinder.cpp %ARGS% -o bin/code_injection__codefinder.o
g++ -c code_injection/signature.cpp %ARGS% -o bin/code_injection__signature.o

g++ -c math/data_types.cpp %ARGSFAST% -o bin/math__data_types.o

:END
g++ bin/* %LARGS% -L client/lua/lua/bin -llua -shared -lws2_32 -static-libgcc -static-libstdc++ -static -luserenv -static -lpthread -static -ladvapi32 -o "C:\Program Files (x86)\Microsoft Games\Halo Custom Edition\controls\chimera.dll"
