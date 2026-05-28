# Beacon Locator BDL - Route-Based Navigation Architecture

## Purpose

This document describes the planned software architecture for the Beacon Locator BDL project.

The current prototype uses a simple linked list route system. This works for a basic sequence such as:


find beacon 1 -> find beacon 2 -> find beacon 3

However, the final game design needs a more flexible route-based navigation system.

The player moves between multiple physical game areas. The same physical beacon can have a different meaning depending on the current route. For example, a beacon near the Build & Test area can be a pass-by beacon during one route, but later become the target beacon for another route.

Because of this, beacon meaning should not be fixed inside a simple linear linked list. Instead, the software should interpret each beacon based on the active route.

-MAIN SOFTWARE PATTERN-

The game is controlled by a finite state machine.

There are two main types of states:

-Mission states
-Navigation state

Mission states handle puzzles, questions, and player input.

The navigation state handles movement between physical locations using BLE beacon signals.


The general flow is:

Mission State
    -> validate player answer
    -> if correct, set next route
    -> enter Navigation State
    -> scan beacons and guide players
    -> confirm arrival
    -> enter next Mission State

//Why the Current SLL System Is Not Enough

The current SLL system works like a simple linear checklist.

It stores a sequence of expected beacons and advances to the next node after a match is found.

This is useful for a prototype, but it has limitations:

The route is fixed.
Beacon meaning is static.
The same beacon cannot easily have different roles in different routes.
It does not clearly separate game logic from navigation logic.
It becomes hard to extend when the game has multiple missions and paths.

The final game needs route-based interpretation:

Physical Beacon ID + Active Route = Beacon Meaning

Example:

Beacon B07 in route MATERIAL_QUESTION_TO_COLLECTION = PASS_BY
Beacon B07 in route COLLECTION_TO_BUILD_TEST = TARGET


ROUTE PROFILE

A RouteProfile describes how beacons should be interpreted during one navigation stage.

Each route contains a list of beacon rules.

Each rule defines:

-beacon ID
-beacon role
-RSSI threshold

Example roles:

IGNORE
PASS_BY
APPROACH
TARGET
WRONG_WAY

This means the same physical beacon can be reused in different routes with different meanings.

RSSI-BASED FEEDBACK

BLE RSSI values are noisy. Therefore, raw RSSI should not be trusted directly.

The navigation module should use averaged RSSI values and track the strongest target signal seen so far.

Important variables:

currentAvgRssi
bestRssi
targetSeen
targetNear

Example:

bestRssi = -72
currentAvgRssi = -66

-66 is stronger than -72, so the player is probably getting closer.

Example:

bestRssi = -58
currentAvgRssi = -72

The signal became much weaker, so the player is probably getting further away.

Recommended starting thresholds:

improvementMargin = 4 dB
gettingFurtherThreshold = 12 dB
wrongDirectionThreshold = 20 dB

These values should be tested and adjusted in the real environment.

PLANNED MODULES

/beacon_parser

Responsible for parsing raw HM-10 scan strings into structured beacon data.

Possible files:

src/beacon/beacon_parser.h
src/beacon/beacon_parser.c
navigation

Responsible for route handling, beacon interpretation, RSSI feedback, and arrival detection.

Possible files:

src/navigation/navigation.h
src/navigation/navigation.c
src/navigation/routes.h
src/navigation/routes.c

/input_buffer
Responsible for collecting keypad input for mission answers.

Possible files:

src/input/input_buffer.h
src/input/input_buffer.c

PLANNED GAME STATES

Possible final states:

STATE_INIT
STATE_BOOT_MENU
STATE_ADMIN_MODE
STATE_NAVIGATION
STATE_EXPERIENCE_MISSION
STATE_MATERIAL_QUESTION
STATE_MATERIAL_COLLECTION
STATE_BUILD_AND_TEST
STATE_FINISH
STATE_SUCCESS

The exact names can still change during implementation.

DESIGN RULE

Mission states should not contain beacon scanning logic.

Navigation state should not contain puzzle logic.

This keeps responsibilities clear:

Mission states = game tasks and answers
Navigation state = physical movement and beacon guidance

RELATED UML DIAGRAMS

The related PlantUML diagrams are stored in:

docs/uml/

Planned diagrams:

main_game_fsm.puml
navigation_state.puml
data_structure.puml
navigation_sequence.puml
route_profile_example.puml