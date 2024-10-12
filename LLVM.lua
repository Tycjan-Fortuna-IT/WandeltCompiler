llvmDir = os.getenv("LLVM_DIR")

function os.capture(cmd, raw)
    local f = assert(io.popen(cmd, 'r'))
    local s = assert(f:read('*a'))

    f:close()

    if raw then return s end
    s = string.gsub(s, '^%s+', '')
    s = string.gsub(s, '%s+$', '')
    s = string.gsub(s, '[\n\r]+', ' ')
    return s
end

function extractLibNames(libsString)
    local libsTable = {}

    for lib in libsString:gmatch("[^%s]+") do
        table.insert(libsTable, lib:match("([^\\]+%.lib)$"))
    end

    return libsTable
end

-- .\llvm-config.exe --libs support core irreader native
local llvmModules = { "support", "core", "irreader", "native" }
local libsToLink = os.capture(llvmDir .. "/bin/llvm-config.exe --libs " .. table.concat(llvmModules, " "))

llvmLibsTable = extractLibNames(libsToLink)