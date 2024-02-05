#pragma once

// THIS FILE ONLY GETS INCLUDED IN A SPECIFIC PART OF Manager.cpp
// ABSOLUTELY DISGUISTING BUT CONVENIENT AS I DON'T WANT TO WRITE
// A MAP IMPORTER. AND I DON'T WANT TO STORE IT DIRECTLY IN
// Manager.cpp TO AVOID CLUTTER

// LEVEL 1
Level* lvl1 = new Level();
lvl1->setSpawn(sf::Vector2f(gameSize.x * .2f, gameSize.y * .8f), 90.f);
lvl1->addTarget(sf::Vector2f(gameSize.x * .2f, gameSize.y * .5f));
lvl1->addTarget(sf::Vector2f(gameSize.x * .5f, gameSize.y * .5f));
lvl1->addTarget(sf::Vector2f(gameSize.x * .8f, gameSize.y * .5f));
lvl1->addWall(sf::Vector2f(0.f, 0.f), sf::Vector2f(gameSize.x, gameSize.y * .1f)); // horizontal border
lvl1->addWall(sf::Vector2f(0.f, gameSize.y * .9f), sf::Vector2f(gameSize.x, gameSize.y * .1f)); // horizontal border
lvl1->addWall(sf::Vector2f(0.f, gameSize.y * .105f), sf::Vector2f(gameSize.y * .1f, gameSize.y * .79f)); // vertical border
lvl1->addWall(sf::Vector2f(gameSize.x - gameSize.y * .1f, gameSize.y * .105f), sf::Vector2f(gameSize.y * .1f, gameSize.y * .79f)); // vertical border

// LEVEL 2
Level* lvl2 = new Level();
lvl2->setSpawn(sf::Vector2f(gameSize.x * .8f, gameSize.y * .2f), 0.f);
lvl2->addTarget(sf::Vector2f(gameSize.x * .2f, gameSize.y * .2f));
lvl2->addTarget(sf::Vector2f(gameSize.x * .2f, gameSize.y * .8f));
lvl2->addTarget(sf::Vector2f(gameSize.x * .3f, gameSize.y * .5f));
lvl2->addWall(sf::Vector2f(gameSize.x * .5f, gameSize.y * .45f), sf::Vector2f(gameSize.x * .5f, gameSize.y * .1f));
lvl2->addWall(sf::Vector2f(0.f, 0.f), sf::Vector2f(gameSize.x, gameSize.y * .1f)); // horizontal border
lvl2->addWall(sf::Vector2f(0.f, gameSize.y * .9f), sf::Vector2f(gameSize.x, gameSize.y * .1f)); // horizontal border
lvl2->addWall(sf::Vector2f(0.f, gameSize.y * .11f), sf::Vector2f(gameSize.y * .1f, gameSize.y * .78f)); // vertical border
lvl2->addWall(sf::Vector2f(gameSize.x - gameSize.y * .1f, gameSize.y * .11f), sf::Vector2f(gameSize.y * .1f, gameSize.y * .78f)); // vertical border

// LEVEL 3
Level* lvl3 = new Level();
lvl3->setSpawn(sf::Vector2f(300.f, 800.f), 90.f);
lvl3->addWall(sf::Vector2f(3, 3), sf::Vector2f(2074, 144));
lvl3->addWall(sf::Vector2f(3, 1003), sf::Vector2f(2074, 164));
lvl3->addWall(sf::Vector2f(3, 153), sf::Vector2f(144, 844));
lvl3->addWall(sf::Vector2f(1903, 153), sf::Vector2f(174, 844));
lvl3->addWall(sf::Vector2f(603, 603), sf::Vector2f(194, 194));
lvl3->addWall(sf::Vector2f(1203, 803), sf::Vector2f(694, 194));
lvl3->addWall(sf::Vector2f(1203, 603), sf::Vector2f(194, 194));
lvl3->addWall(sf::Vector2f(1403, 603), sf::Vector2f(494, 194));
lvl3->addTarget(sf::Vector2f(250, 250));
lvl3->addTarget(sf::Vector2f(900, 650));
lvl3->addTarget(sf::Vector2f(1350, 250));

// LEVEL 4
Level* lvl4 = new Level();
lvl4->setSpawn(sf::Vector2f(250.f, 250.f), 180.f);
lvl4->addWall(sf::Vector2f(2, 2), sf::Vector2f(2076, 146));
lvl4->addWall(sf::Vector2f(2, 1002), sf::Vector2f(2076, 166));
lvl4->addWall(sf::Vector2f(2, 152), sf::Vector2f(146, 846));
lvl4->addWall(sf::Vector2f(1902, 152), sf::Vector2f(176, 846));
lvl4->addWall(sf::Vector2f(702, 402), sf::Vector2f(196, 96));
lvl4->addWall(sf::Vector2f(802, 502), sf::Vector2f(246, 96));
lvl4->addWall(sf::Vector2f(702, 602), sf::Vector2f(196, 96));
lvl4->addWall(sf::Vector2f(152, 902), sf::Vector2f(346, 96));
lvl4->addWall(sf::Vector2f(502, 902), sf::Vector2f(96, 96));
lvl4->addWall(sf::Vector2f(602, 902), sf::Vector2f(346, 96));
lvl4->addWall(sf::Vector2f(952, 902), sf::Vector2f(96, 96));
lvl4->addWall(sf::Vector2f(1052, 902), sf::Vector2f(346, 96));
lvl4->addWall(sf::Vector2f(1402, 902), sf::Vector2f(96, 96));
lvl4->addWall(sf::Vector2f(1502, 902), sf::Vector2f(396, 96));
lvl4->addTarget(sf::Vector2f(850, 250));
lvl4->addTarget(sf::Vector2f(1200, 400));
lvl4->addTarget(sf::Vector2f(1250, 550));
lvl4->addTarget(sf::Vector2f(1200, 700));
lvl4->addTarget(sf::Vector2f(850, 850));

// LEVEL 5
Level* lvl5 = new Level();
lvl5->setSpawn(sf::Vector2f(1040.f, 585.f), 0);
lvl5->addWall(sf::Vector2f(2, 4), sf::Vector2f(2072, 142));
lvl5->addWall(sf::Vector2f(2, 154), sf::Vector2f(142, 842));
lvl5->addWall(sf::Vector2f(2, 1004), sf::Vector2f(2072, 162));
lvl5->addWall(sf::Vector2f(1902, 154), sf::Vector2f(172, 842));
lvl5->addWall(sf::Vector2f(502, 404), sf::Vector2f(192, 92));
lvl5->addWall(sf::Vector2f(602, 504), sf::Vector2f(192, 92));
lvl5->addWall(sf::Vector2f(502, 604), sf::Vector2f(192, 92));
lvl5->addWall(sf::Vector2f(402, 504), sf::Vector2f(192, 92));
lvl5->addWall(sf::Vector2f(302, 604), sf::Vector2f(192, 92));
lvl5->addWall(sf::Vector2f(302, 404), sf::Vector2f(192, 92));
lvl5->addWall(sf::Vector2f(202, 504), sf::Vector2f(192, 92));
lvl5->addWall(sf::Vector2f(1402, 404), sf::Vector2f(192, 292));
lvl5->addWall(sf::Vector2f(1552, 404), sf::Vector2f(142, 292));
lvl5->addWall(sf::Vector2f(1652, 404), sf::Vector2f(142, 292));
lvl5->addWall(sf::Vector2f(1002, 854), sf::Vector2f(442, 42));
lvl5->addWall(sf::Vector2f(1152, 604), sf::Vector2f(42, 342));
lvl5->addWall(sf::Vector2f(1002, 704), sf::Vector2f(442, 42));
lvl5->addTarget(sf::Vector2f(1050, 350));
lvl5->addTarget(sf::Vector2f(1300, 300));
lvl5->addTarget(sf::Vector2f(900, 700));
