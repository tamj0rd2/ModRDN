----------------------------------------------------------------------
-- constants
----------------------------------------------------------------------

-- The different line styles are declared by the game.
-- STYLE_NORMAL
-- STYLE_TITLE
-- STYLE_BLANK

-- configuration
config = {}
config.txtHeight 		= 0.03
config.txtWidth	 		= 0.3
config.txtFont			= "century"
config.medianWidth		= 0.02
config.scrollLinesPerSec	= 1.0

----------------------------------------------------------------------
-- function declarations
----------------------------------------------------------------------

credits = {}
linenum = 1

addcredit = function( line )
	credits[linenum] = line
	linenum = linenum + 1
end

----------------------------------------------------------------------
-- data
----------------------------------------------------------------------
-- Note: use "$#####" to enter localized text
--

credits = {}

addcredit( { STYLE_TITLE, "Design" } )
addcredit( { STYLE_NORMAL, "Original Concept", "Alex Garden" } )
addcredit( { STYLE_NORMAL, "Lead Designer", "Jay Wilson" } )
addcredit( { STYLE_NORMAL, "Designers", "Quinn Duffy" } )
addcredit( { STYLE_NORMAL, "", "Andrew Chambers" } )
addcredit( { STYLE_BLANK } )

addcredit( { STYLE_TITLE, "Programming" } )
addcredit( { STYLE_NORMAL, "Lead Programmer", "Shane Alfreds" } )
addcredit( { STYLE_NORMAL, "Programmers", "Drew Dunlop" } )
addcredit( { STYLE_NORMAL, "", "Shelby Hubick" } )
addcredit( { STYLE_NORMAL, "", "Francois Laberge" } )
addcredit( { STYLE_NORMAL, "", "Cedric Lee" } )
addcredit( { STYLE_NORMAL, "", "Dominic Mathieu" } )
addcredit( { STYLE_NORMAL, "", "Johnathan Skinner" } )
addcredit( { STYLE_NORMAL, "", "David Swinerd" } )
addcredit( { STYLE_NORMAL, "", "Ian Thomson" } )
addcredit( { STYLE_BLANK } )

addcredit( { STYLE_TITLE, "Art" } )
addcredit( { STYLE_NORMAL, "Art Director", "Andy Lang" } )
addcredit( { STYLE_NORMAL, "Lead Artist", "Arthur Shimizu" } )
addcredit( { STYLE_NORMAL, "Artists", "Nick Carota" } )
addcredit( { STYLE_NORMAL, "", "Craig Calvert" } )
addcredit( { STYLE_NORMAL, "", "Ronel Mendoza" } )
addcredit( { STYLE_NORMAL, "", "Erin Olorenshaw" } )
addcredit( { STYLE_NORMAL, "", "Angie Pytlewski" } )
addcredit( { STYLE_NORMAL, "", "Joe Schober" } )
addcredit( { STYLE_BLANK } )

addcredit( { STYLE_TITLE, "Sound" } )
addcredit( { STYLE_NORMAL, "Sound Director", "Crispin Hands" } )
addcredit( { STYLE_BLANK } )

addcredit( { STYLE_TITLE, "Production" } )
addcredit( { STYLE_NORMAL, "Producer", "John Johnson" } )
addcredit( { STYLE_NORMAL, "Assistant Producer", "Jennifer Hawke" } )
addcredit( { STYLE_BLANK } )

----------------------------------------------------------------------
-- test 
----------------------------------------------------------------------

-- prline = function( line )
-- 	local str = ""
-- 	local k
-- 	local v
-- 
-- 	k, v = next( line )
-- 	while (k ~= nil) do
-- 		if (v == "") then
-- 			v = "-"
-- 		end
-- 		str = str .. v .. "\t"
-- 		k, v = next( line, k )
-- 	end
-- 
-- 	print( str )
-- end
-- 
-- test = function()
-- 	local k
-- 	local v
-- 
-- 	k, v = next( credits )
-- 	while (k ~= nil) do
-- 		prline( v )
-- 		k, v = next( credits, k )
-- 	end	
-- end
