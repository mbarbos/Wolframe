local function normalizeName( name)
	return name:gsub("[^%s]+", string.lower):gsub("[%-()]+", " "):gsub("^%s+", ""):gsub("%s+$", ""):gsub("%s+", " ")
end

local function content_value( v, itr)
	if v then
		return v
	end
	for v,t in itr do
		if t then
			return nil
		end
		if v then
			return v
		end
		return nil
	end
end

local function insert_itr( tablename, parentid, itr)
	local id = 1
	local name = ""
	local nname = ""
	for v,t in itr do
		if (t == "name") then
			name = content_value( v, itr)
			nname = normalizeName( name)
			id = formfunction( "add" .. tablename)( {name=v, normalizedName=nname, parentid=parentid} ):table().ID
		elseif (t == "node") then
			insert_itr( tablename, id, scope( itr))
		end
	end
	return id
end

local function insert_topnode( tablename, name, parentid)
	local nname = normalizeName( name)
	if not parentid then
		formfunction( "add" .. tablename .. "Root")( {normalizedName=nname, name=name} )
		return 1
	else
		local id = formfunction( "add" .. tablename)( {normalizedName=nname, name=name, parentid=parentid} ):table().ID
		return id
	end
end

local function insert_tree_topnode( tablename, itr)
	local parentid = nil
	local id = 1
	local name = nil
	for v,t in itr do
		if (t == "parent") then
			parentid = tonumber( v)
		elseif (t == "name") then
			name = content_value( v, itr)
		elseif (t == "node") then
			if name then
				id = insert_topnode( tablename, name, parentid)
				name = nil
			end
			insert_itr( tablename, id, scope( itr))
		end
	end
	if name then
		insert_topnode( tablename, name, parentid)
	end
end

local function get_tree( tablename, parentid)
	local t = formfunction( "selectSub" .. tablename)( {id=parentid} ):table()["node"] or {}
	local a = {}
	for i,v in pairs( t) do
		table.insert( a, tonumber( v.ID), { name=v.name, parent=tonumber(v.parent), children = {} } )
	end
	for i,v in pairs( a) do
		if i ~= parentid and v.parent then
			table.insert( a[ v.parent ].children, i )
		end
	end
	return a
end

local function print_tree( tree, nodeid, indent)
	if (indent ~= "") then
		output:print( "\n" .. indent)
	end
	output:opentag( "tree" )
	output:opentag( "item" )
	output:print( nodeid, "id")
	output:print( "\n" .. indent )
	output:opentag( "category" )
	output:print( tree[ nodeid ].name )
	-- more attributes of category follow here, like description
	output:closetag( )
	local n = 0
	for i,v in pairs( tree[ nodeid].children) do
		print_tree( tree, v, indent .. "\t")
		n = n + 1
	end
	if n > 0 then
		output:print( "\n" .. indent)
	end
	output:closetag( )
	output:closetag()
end

local function select_tree( tablename, itr)
	filter().empty = false
	for v,t in itr do
		if t == "id" then
			local id = tonumber( v)
			print_tree( get_tree( tablename, id), id, "")
		end
	end
end

local function select_node( tablename, elementname, itr)
	filter().empty = false
	for v,t in itr do
		if t == "id" then
			output:opentag( elementname)
			output:print( v, "id")
			local r = formfunction( "select" .. tablename)( {id=v} )
			output:print( r:get())
			output:closetag()
		end
	end
end

local function edit_node( tablename, itr)
	local name = nil;
	local nname = nil;
	local id = nil;
	for v,t in itr do
		if t == "id" then
			id = v
		elseif t ==  "name" then
			name = content_value( v, itr)
			nname = normalizeName( name)
		end
	end
	formfunction( "update" .. tablename)( {normalizedName=nname, name=name, id=id} )
end

local function delete_node( tablename, itr)
	local id = nil;
	for v,t in itr do
		if t == "id" then
			id = v
		end
	end
	formfunction( "delete" .. tablename)( {id=id} )
end

local function create_node( tablename, itr)
	local name = nil;
	local parentid = nil;
	for v,t in itr do
		if t == "id" then
			parentid = v
		elseif t == "parent" then
			parentid = v
		elseif t ==  "name" then
			name = content_value( v, itr)
			nname = normalizeName( name)
		end
	end
	insert_topnode( tablename, name, parentid)
end

local function add_tree( tablename, itr)
	filter().empty = false
	for v,t in itr do
		if t == "node" then
			insert_tree_topnode( tablename, scope( itr))
		end
	end
end

function CategoryHierarchyRequest()
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	select_tree( "Category", input:get())
end

function FeatureHierarchyRequest()
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	select_tree( "Feature", input:get())
end

function pushCategoryHierarchy()
	add_tree( "Category", input:get())
end

function pushFeatureHierarchy()
	add_tree( "Feature", input:get())
end

function CategoryRequest()
	output:as( "node SYSTEM 'Category.simpleform'")
	select_node( "Category", "category", input:get())
end

function FeatureRequest()
	output:as( "node SYSTEM 'Feature.simpleform'")
	select_node( "Feature", "feature", input:get())
end

function readCategory()
	print_tree( get_tree( "Category", 1), 1, "")
end

function editCategory()
	edit_node( "Category", input:get())
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	print_tree( get_tree( "Category", 1), 1, "")
end

function editFeature()
	edit_node( "Feature", input:get())
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	print_tree( get_tree( "Feature", 1), 1, "")
end

function deleteCategory()
	delete_node( "Category", input:get())
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	print_tree( get_tree( "Category", 1), 1, "")
end

function deleteFeature()
	delete_node( "Feature", input:get())
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	print_tree( get_tree( "Feature", 1), 1, "")
end

function createCategory()
	create_node( "Category", input:get())
	output:as( "node SYSTEM 'CategoryHierarchy.simpleform'")
	print_tree( get_tree( "Category", 1), 1, "")
end

function createFeature()
	create_node( "Feature", input:get())
	output:as( "node SYSTEM 'FeatureHierarchy.simpleform'")
	print_tree( get_tree( "Feature", 1), 1, "")
end

