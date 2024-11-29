
#set(SRCDIR ${CMAKE_CURRENT_SOURCE_DIR})

message("Model source")

set(SRC_MODEL 
  dog.cpp
  game.cpp
  game_session.cpp
  #loot.cpp
  loot_generator.cpp
  model_map.cpp
  player.cpp
  player_list.cpp
  tag_invokers.cpp
  token_generator.cpp
)

set(HEADER_MODEL 
  building.h
  dog.h
  game.h
  game_session.h
  geometry.h
  loot.h
  loot_generator.h
  model_map.h
  office.h
  player.h
  player_list.h
  road.h
  tag_invokers.h
  token_generator.h
)

target_sources(${EXECUTE} PUBLIC ${SRC_MODEL} ${HEADER_MODEL})