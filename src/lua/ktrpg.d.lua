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

---@class EntityInstance
local EntityInstance = {}

---@return integer id
function EntityInstance:get_id() end

---@param component IntComponentInstance
---@param value integer
---@overload fun(self: EntityInstance, component: IntComponentInstance, value:integer): nil,string
---@overload fun(self: EntityInstance, component: NumComponentInstance, value:number): number
---@overload fun(self: EntityInstance, component: NumComponentInstance, value:number): nil,string
---@overload fun(self: EntityInstance, component: TagComponentInstance, value:integer): integer
---@overload fun(self: EntityInstance, component: TagComponentInstance, value:integer): nil,string
---@overload fun(self: EntityInstance, component: StrComponentInstance, value:string): string
---@overload fun(self: EntityInstance, component: StrComponentInstance, value:string): nil,string
function EntityInstance:set(component, value) end

---@param component IntComponentInstance
---@overload fun(self: EntityInstance, component: IntComponentInstance): nil, string
---@overload fun(self: EntityInstance, component: NumComponentInstance): number
---@overload fun(self: EntityInstance, component: NumComponentInstance): nil, string
---@overload fun(self: EntityInstance, component: TagComponentInstance): integer
---@overload fun(self: EntityInstance, component: TagComponentInstance): nil, string
---@overload fun(self: EntityInstance, component: StrComponentInstance): string
---@overload fun(self: EntityInstance, component: StrComponentInstance): nil, string
---@return integer
function EntityInstance:get(component) end

---@param component ComponentInstance
---@return boolean
---@overload fun(self:EntityInstance, component:ComponentInstance): false, string
function EntityInstance:remove(component) end

---@return boolean
---@overload fun(self:EntityInstance): false, string
function EntityInstance:kill() end

---@class Entity
local Entity = {}

---@return EntityInstance[] entities
---@return integer entities_count
function Entity:all() end

---@param id integer
---@return EntityInstance entity
---@overload fun(self: Entity, id: integer):nil, string
function Entity:by_id(id) end

---@return EntityInstance entity
---@overload fun(self: Entity):nil, string
function Entity:new() end

---@param limit integer
---@return integer limit
---@overload fun(self: Entity, limit: integer): nil, string
function Entity:set_limit(limit) end

---@return integer limit
function Entity:get_limit() end

---@class SystemInstance
local SystemInstance = {}

---@return string name
function SystemInstance:get_name() end

---@param ... ComponentInstance
---@return SystemInstance
function SystemInstance:requires(...) end

---@param ... ComponentInstance
---@return SystemInstance
function SystemInstance:excludes(...) end

---@param mask integer
---@return SystemInstance
function SystemInstance:set_mask(mask) end

---@return integer mask
function SystemInstance:get_mask() end

---@return SystemInstance
function SystemInstance:enable() end

---@return SystemInstance
function SystemInstance:disable() end

---@alias SystemInstanceCallback fun(entities: EntityInstance[], entities_count: integer)

---@param callback SystemInstanceCallback
---@return SystemInstance
function SystemInstance:on_run(callback) end

---@return SystemInstance
function SystemInstance:run() end

---@return integer entities_count
function SystemInstance:get_entity_count() end

---@class System
local System = {}

---@param name string
---@return SystemInstance
---@overload fun(self: System, name: string): nil, string
function System:new(name) end

---@param limit integer
---@return integer limit
---@overload fun(self: System, limit: integer): nil, string
function System:set_limit(limit) end

---@return integer limit
function System:get_limit() end

---@class ktrpg
local ktrpg = {
  Entity = Entity,
  Component = Component,
  System = System,
}

return ktrpg
