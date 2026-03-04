---@meta

---@class Util
local Util = {}

-- read", util_read},
-- readchar", util_readchar},
-- ask_yn", util_ask_yn},
-- writenl", util_writenl},
-- write", util_write},
-- sleep", util_sleep},

---@param length integer
---@return string
function Util.read(length) end

---@return string
function Util.readchar() end

---@param question integer
---@return boolean
function Util.ask_yn(question) end

---@param text string
---@overload fun()
function Util.writenl(text) end

---@param text string
function Util.write(text) end

---@param seconds integer
function Util.sleep(seconds) end
