# Army Creator

An army list creator for Warhammer Fantasy Battles (WHFB) built using [QtCreator](https://www.qt.io/) with the
core logic programmed in compliance with the C++17 standard. Currently only WHFB 8th Edition is supported but over
time it will be changed to include 6th and 7th Edition rules and army books.

This army list creator is designed to be easy to use avoiding the use of any external dependencies, this includes
not requiring the user to load in separate files for the army book and magic item data. To achieve this, the software
relies on the use of the [Qt Resource System](http://doc.qt.io/qt-5/resources.html) for handling the unit and item
data. In practical terms this means that the user only needs to select the army/faction of their choosing from the
drop-down list to start making an army list for this faction.

## Legal/Copyright Notice

Warhammer Army Creator ("The Software") is completely unofficial and is in no way endorsed by Games Workshop Ltd.
Warhammer, Warhammer Fantasy Battles and all the associated names and content used in The Software are registered
trademarks of Games Workshop Ltd. All data used by The Software is either developer-generated or user-generated
content and is not supported by Games Workshop Ltd. in any way. All Rights Reserved.

All content within the directories: `rosters/`, `magic_items/` and `faction_items/`, contain data which wholly belongs
to Games Workshop Ltd. By using this software, cloning this repository and/or viewing the contents of any files
within these directories, users and/or contributors acknowledge that they are solely responsible for ensuring they
legally own a copy of the Warhammer Fantasy rulebook and the Warhammer Armies book that a given data file is
intended to represent.
