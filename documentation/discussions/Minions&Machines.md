# Minions & Machines

## Minions
### General Approach
- made up of some combination of parts, must be a minimum of one part
- parts are held in a Construct, which is an array/list of parts
- movement is determined by a path finding system which gives the minions velocity and adds it to is postion to give a new position
- the parts all move relative to a root part, which **may** be the first part in the assembly.
- once the part positions are determined, the textures for each part will be mapped to their respective positions
- the combination of parts will determine the minions stats, such as health, attack, etc. For now this will be an Attributes class (but will probably end up being divided)
- 
### Components Needed
- CMinion
	- Tells the system that this entity is a Minion as opposed to a Machine
- CConstruct
	- Contains a list of parts
- CTransform
	- Contains a position vector and potentially a default velocity vector
- CAttributes
	- Contains the stats of the minion


### Systems Needed
- SPathfinding
	- Takes a minion and a target and returns a velocity vector
- SMovement
	- Takes a minion and a velocity vector and updates the position of the minion
- SRender
	- Renders the minion

### Classes Needed
- Construct
- Part
	- Contains sockets for attachment

## Machines
### What should machines do?
- craft parts/items
- produce minions
- generate magic 
- move items
- process resources

### General Approach
- Machines need to meet certain attribute requirements before they are fit for a purpose
- once crafted on the draft table, any available functions can be selected


# Scenes
## General Approach

## Scene_MainScreen