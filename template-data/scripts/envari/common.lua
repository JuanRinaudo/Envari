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

function lpad(s, l, c)
    local res = string.rep(c or ' ', l - #s) .. s

    return res, res ~= s
end

function rpad(s, l, c)
    local res = s .. string.rep(c or ' ', l - #s)

    return res, res ~= s
end

function pad(s, l, c)
    c = c or ' '

    local res1, stat1 = rpad(s,    (l / 2) + #s, c) -- pad to half-length + the length of s
    local res2, stat2 = lpad(res1,  l,           c) -- right-pad our left-padded string to the full length

    return res2, stat1 or stat2
end