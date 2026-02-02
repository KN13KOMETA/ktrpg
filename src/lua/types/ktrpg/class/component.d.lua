---@meta

---@class ComponentInstance
local ComponentInstance = {}

---@return string name
function ComponentInstance:get_name() end

---@class IntComponentInstance: ComponentInstance
local IntComponentInstance = {}

---@class NumComponentInstance: ComponentInstance
local NumComponentInstance = {}

---@class TagComponentInstance: ComponentInstance
local TagComponentInstance = {}

---@class StrComponentInstance: ComponentInstance
local StrComponentInstance = {}

---@class Component
local Component = {}

---@param type string
---@param name string
---@return nil, string
---@overload fun(self: Component, type: "int", name): IntComponentInstance
---@overload fun(self: Component, type: "int", name): nil, string
---@overload fun(self: Component, type: "num", name): NumComponentInstance
---@overload fun(self: Component, type: "num", name): nil, string
---@overload fun(self: Component, type: "tag", name): TagComponentInstance
---@overload fun(self: Component, type: "tag", name): nil, string
---@overload fun(self: Component, type: "str", name): StrComponentInstance
---@overload fun(self: Component, type: "str", name): nil, string
function Component:new(type, name) end

---@param limit integer
---@return integer limit
---@overload fun(self: Component, limit: integer): nil, string
function Component:set_limit(limit) end

---@return integer limit
function Component:get_limit() end
