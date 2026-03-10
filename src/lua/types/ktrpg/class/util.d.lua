---@meta

---@class Util
local Util = {}

---@param length integer
---@return string
function Util.read(length) end

---@return string
function Util.readchar() end

---@param question integer
---@return boolean
function Util.ask_yn(question) end

---@param content any
---@overload fun()
function Util.writenl(content) end

---@param content any
function Util.write(content) end

---@param seconds integer
function Util.sleep(seconds) end

function Util.exit() end
