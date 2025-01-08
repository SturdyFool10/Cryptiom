local p = premake

p.modules.export_compile_commands = {}
local m = p.modules.export_compile_commands

local workspace = p.workspace
local project = p.project

function m.getToolset(cfg)
    return p.tools[cfg.toolset or 'gcc']
end

function m.getCommonFlags(cfg)
    local flags = {}

    -- Add -MD -MP flags
    table.insert(flags, "-MD")
    table.insert(flags, "-MP")

    -- Add defines
    for _, define in ipairs(cfg.defines) do
        table.insert(flags, "-D" .. define)
    end

    -- Add include directories
    for _, includedir in ipairs(cfg.includedirs) do
        table.insert(flags, "-I" .. includedir)
    end

    -- Add C/C++ dialect flags
    if cfg.cdialect then
        table.insert(flags, "-std=" .. cfg.cdialect)
    elseif cfg.cppdialect then
        table.insert(flags, "-std=" .. cfg.cppdialect)
    else
        table.insert(flags, "-std=c++23")
    end

    -- Add optimization level
    if cfg.optimize then
        if cfg.optimize == "Debug" then
            table.insert(flags, "-O0")
        elseif cfg.optimize == "Size" then
            table.insert(flags, "-Os")
        elseif cfg.optimize == "Speed" then
            table.insert(flags, "-O2")
        elseif cfg.optimize == "Full" then
            table.insert(flags, "-O3")
        end
    end

    -- Add debug info if specified
    if cfg.symbols then
        table.insert(flags, "-g")
    end

    -- Add architecture
    if cfg.architecture == "x86_64" then
        table.insert(flags, "-m64")
    elseif cfg.architecture == "x86" then
        table.insert(flags, "-m32")
    end

    return flags
end

function m.getObjectPath(prj, cfg, node)
    return path.join(cfg.objdir, node.objname)
end

function m.getDependenciesPath(prj, cfg, node)
    return path.join(cfg.objdir, node.objname .. ".d")
end

function m.getFileFlags(prj, cfg, node)
    local flags = m.getCommonFlags(cfg)
    table.insert(flags, '-o')
    table.insert(flags, m.getObjectPath(prj, cfg, node))
    table.insert(flags, '-MF')
    table.insert(flags, m.getDependenciesPath(prj, cfg, node))
    table.insert(flags, '-c')
    table.insert(flags, node.abspath)
    return flags
end

function m.generateCompileCommand(prj, cfg, node)
    local compiler = iif(p.project.iscpp(prj), "c++", "cc")
    return {
        directory = cfg.buildtarget.directory,
        file = node.abspath,
        command = compiler .. ' ' .. table.concat(m.getFileFlags(prj, cfg, node), ' ')
    }
end

function m.includeFile(prj, node, depth)
    return p.path.iscppfile(node.abspath) or p.path.iscfile(node.abspath)
end

function m.getConfig(prj)
    if p._OPTIONS['export-compile-commands-config'] then
        return project.getconfig(prj, p._OPTIONS['export-compile-commands-config'],
            p._OPTIONS['export-compile-commands-platform'])
    end
    for cfg in project.eachconfig(prj) do
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
                cfgCmds[cfgKey] = cfgCmds[cfgKey] or {}
                local commands = m.getProjectCommands(prj, cfg)
                for _, cmd in ipairs(commands) do
                    table.insert(cfgCmds[cfgKey], cmd)
                end
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

p.newaction {
    trigger = 'export-compile-commands',
    description = 'Export compiler commands in JSON Compilation Database Format',
    execute = execute
}

return m
