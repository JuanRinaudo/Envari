function Load()
    LoadLibrary(SOL_LIBRARY_PACKAGE);
    LoadLibrary(SOL_LIBRARY_OS);
    LoadLibrary(SOL_LIBRARY_IO);
    LoadLibrary(SOL_LIBRARY_BASE);
    LoadLibrary(SOL_LIBRARY_STRING);
    LoadLibrary(SOL_LIBRARY_MATH);
    LoadLibrary(SOL_LIBRARY_TABLE);
    LoadLibrary(SOL_LIBRARY_COROUTINE);
end

function Update()
    DrawSetLayer(0, false);
    DrawDefaultTransform();

    DrawClear(0, 0, 0, 1);
    DrawColor(1, 1, 1, 1);

    DrawRectangle(32, 32, 32, 32);
end

function Unload()

end

function End()
    
end