-- Lab

-- inherit from lab data
dofilepath("data:art/ebps/defstats5.lua")

-- extend gameattributes table
ext = gameattributes;

-- baseline optimization
ext.min_triangle_count=1300

ext.sight_radius = 40
