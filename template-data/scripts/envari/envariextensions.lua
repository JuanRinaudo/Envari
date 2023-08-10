queue = {};

function queue.new()
    local list = {};
    list.first = 0;
    list.last = -1;
    list.size = 0;
    return list;
end

function queue.push(list, value)
    list.last = list.last + 1;
    list.size = list.size + 1;
    list[list.last] = value;
end

function queue.pop(list)
    if(list.size > 0) then
        local value = list[list.first];
        list[list.first] = nil;
        list.size = list.size - 1;
        list.first = list.first + 1;
        return value;
    else
        return nil;
    end
end

function math.clamp(x, min, max)
    return math.max(math.min(x, max), min);
end