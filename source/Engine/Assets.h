#ifndef ASSETS_H
#define ASSETS_H

static TextAsset LoadTextToMemory(const char* filepath, const char* mode)
{
	TextAsset asset = {};
	asset.data = (char*)LoadFileToMemory(filepath, mode, &asset.size);
	return asset;
}

#endif