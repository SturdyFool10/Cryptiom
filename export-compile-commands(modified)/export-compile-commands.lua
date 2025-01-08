local p = premake

p.modules.export_compile_commands = {}
local m = p.modules.export_compile_commands

local workspace = p.workspace
local project = p.project

-- Lookup tables for C++ dialect flags
local cpp_dialects = {
    gcc = {
        ["C++98"] = "-std=c++98",
        ["C++11"] = "-std=c++11",
        ["C++14"] = "-std=c++14",
        ["C++17"] = "-std=c++17",
        ["C++20"] = "-std=c++20",
        ["C++23"] = "-std=c++23"
    },
    clang = {
        ["C++98"] = "-std=c++98",
        ["C++11"] = "-std=c++11",
        ["C++14"] = "-std=c++14",
        ["C++17"] = "-std=c++17",
        ["C++20"] = "-std=c++20",
        ["C++23"] = "-std=c++2b"
    },
    msc = {
        ["C++98"] = "/std:c++98",
        ["C++11"] = "/std:c++11",
        ["C++14"] = "/std:c++14",
        ["C++17"] = "/std:c++17",
        ["C++20"] = "/std:c++20",
        ["C++23"] = "/std:c++latest"
    }
}

function m.getToolset(cfg)
    return p.tools[cfg.toolset or 'gcc']
end

function m.getIncludeDirs(cfg)
    local flags = {}
    local toolset = m.getToolset(cfg)
    local includeDirFlag = (toolset.name == "msc") and "/I" or "-I"
    local sysIncludeDirFlag = (toolset.name == "msc") and "/I" or "-isystem"

    for _, dir in ipairs(cfg.includedirs) do
        table.insert(flags, includeDirFlag .. p.quoted(dir))
    end
    for _, dir in ipairs(cfg.sysincludedir or {}) do
        table.insert(result, sysIncludeDirFlag .. p.quoted(dir))
    end
    return flags
end

function m.getCppDialectFlag(cfg)
    local toolset = m.getToolset(cfg)
    local dialectFlags = cpp_dialects[toolset.name] or cpp_dialects.gcc
    return cfg.cppdialect and dialectFlags[cfg.cppdialect] or nil
end

function m.getCommonFlags(cfg)
    local toolset = m.getToolset(cfg)
    local flags = toolset.getcppflags(cfg)
    flags = table.join(flags, toolset.getdefines(cfg.defines))
    flags = table.join(flags, toolset.getundefines(cfg.undefines))
    flags = table.join(flags, m.getIncludeDirs(cfg))
    flags = table.join(flags, toolset.getcflags(cfg))

    local dialectFlag = m.getCppDialectFlag(cfg)
    if dialectFlag then
        flags = table.join(flags, { dialectFlag })
    end

    return table.join(flags, cfg.buildoptions)
end

function m.getObjectPath(prj, cfg, node)
    local ext = (m.getToolset(cfg).name == "msc") and ".obj" or ".o"
    return path.join(cfg.objdir, path.appendExtension(node.objname, ext))
end

function m.getDependenciesPath(prj, cfg, node)
    return path.join(cfg.objdir, path.appendExtension(node.objname, '.d'))
end

function m.getFileFlags(prj, cfg, node)
    local toolset = m.getToolset(cfg)
    local flags = m.getCommonFlags(cfg)

    if toolset.name == "msc" then
        return table.join(flags, {
            "/Fo" .. m.getObjectPath(prj, cfg, node),
            "/c", node.abspath
        })
    else
        return table.join(flags, {
            '-o', m.getObjectPath(prj, cfg, node),
            '-MF', m.getDependenciesPath(prj, cfg, node),
            '-c', node.abspath
        })
    end
end

function m.generateCompileCommand(prj, cfg, node)
    local toolset = m.getToolset(cfg)
    local compiler = toolset.name == "msc" and "cl.exe" or (toolset.name == "clang" and "clang++" or "g++")

    return {
        directory = prj.location,
        file = node.abspath,
        command = compiler .. ' ' .. table.concat(m.getFileFlags(prj, cfg, node), ' ')
    }
end

function m.includeFile(prj, node, depth)
    return path.iscppfile(node.abspath)
end

function m.getConfig(prj)
    if _OPTIONS['export-compile-commands-config'] then
        return project.getconfig(prj, _OPTIONS['export-compile-commands-config'],
            _OPTIONS['export-compile-commands-platform'])
    end
    for cfg in project.eachconfig(prj) do
        -- just use the first configuration which is usually "Debug"
        return cfg
    end
end

function m.getProjectCommands(prj, cfg)
    local tr = project.getsourcetree(prj)
    local cmds = {}
    p.tree.traverse(tr, {
        onleaf = function(node, depth)
            if not m.includeFile(prj, node, depth) then
                return
            end
            table.insert(cmds, m.generateCompileCommand(prj, cfg, node))
        end
    })
    return cmds
end

local function execute()
    for wks in p.global.eachWorkspace() do
        local cfgCmds = {}
        for prj in workspace.eachproject(wks) do
            for cfg in project.eachconfig(prj) do
                local cfgKey = string.format('%s', cfg.shortname)
                if not cfgCmds[cfgKey] then
                    cfgCmds[cfgKey] = {}
                end
                cfgCmds[cfgKey] = table.join(cfgCmds[cfgKey], m.getProjectCommands(prj, cfg))
            end
        end
        for cfgKey, cmds in pairs(cfgCmds) do
            local outfile = string.format('compile_commands/%s.json', cfgKey)
            p.generate(wks, outfile, function(wks)
                p.w('[')
                for i = 1, #cmds do
                    local item = cmds[i]
                    local command = string.format([[
          {
            "directory": "%s",
            "file": "%s",
            "command": "%s"
          }]],
                        item.directory,
                        item.file,
                        item.command:gsub('\\', '\\\\'):gsub('"', '\\"'))
                    if i > 1 then
                        p.w(',')
                    end
                    p.w(command)
                end
                p.w(']')
            end)
        end
    end
end

newaction {
    trigger = 'export-compile-commands',
    description = 'Export compiler commands in JSON Compilation Database Format',
    execute = execute
}

return m
