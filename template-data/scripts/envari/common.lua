function shallowcopy(t)
    local t2 = {}
    for k, v in pairs(t) do
        t2[k] = v
    end
    return t2
end

function deepcopy(t)
    if type(t) ~= 'table' then
        return t
    end
    local t2 = {}
    for k, v in pairs(t) do
        t2[k] = deepcopy(v)
    end
    return t2
end