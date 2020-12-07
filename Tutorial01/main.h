#pragma once

#include <windows.h>
#include <windowsx.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include <DirectXCollision.h>
#include "DDSTextureLoader.h"
#include "resource.h"
#include <iostream>
#include <string>

#include "DrawableGameObject.h"
#include "DrawableObjectCube.h"
#include "DrawableGameObjectPlane.h"
#include "structures.h"

#include "imgui.h"
#include "examples/imgui_impl_win32.h"
#include "examples/imgui_impl_dx11.h"
#include <vector>

#include "CameraManager.h"
#include "Camera.h"
#include "Time.h"

using namespace std;

vector<DrawableGameObject*> vecDrawables;
