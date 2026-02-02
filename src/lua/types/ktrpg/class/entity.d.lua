---@meta

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
