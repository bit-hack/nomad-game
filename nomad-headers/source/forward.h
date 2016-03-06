#pragma once

#include "../../nomad-util/source/ref.h"

namespace event {
struct event_header_t;
struct event_t;
} // namespace event

namespace object
{
struct object_t;
typedef ref_t<object_t> object_ref_t;
} // namespace object

namespace game
{
struct game_t;
struct game_view_t;
struct cue_t;
struct map_info_t;
struct generator_t;
} // namespace nomad

namespace player
{
struct stream_t;
struct player_t;
} // namespace player
