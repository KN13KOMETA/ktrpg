---@meta

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
