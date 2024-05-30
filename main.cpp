#include <Novice.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <assert.h>
#include <imgui.h>
#include <algorithm>
#include "Matrix4x4.h"
#include "Vector3.h"
#include "VectorMatrix.h"
#include "Sphere.h"
#include "Segment.h"
#include "AABB.h"

using namespace std;

static const int kWindowWidth = 1280;
static const int kWindowHeight = 720;

//Gridを表示する疑似コード
static void DrawGrid(const Matrix4x4& ViewProjectionMatrix, const Matrix4x4& ViewportMatrix)
{
	const float    kGridHalfWidth = 2.0f;                                        //Gridの半分の幅
	const uint32_t kSubdivision = 10;                                        //分割数
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);    //1つ分の長さ

	//水平方向の線を描画
	for (uint32_t xIndex = 0; xIndex <= kSubdivision; xIndex++)
	{
		//X軸上の座標
		float posX = -kGridHalfWidth + kGridEvery * xIndex;

		//始点と終点
		Vector3 start = { posX, 0.0f, -kGridHalfWidth };
		Vector3 end = { posX, 0.0f, kGridHalfWidth };
		// ワールド座標系 -> スクリーン座標系まで変換をかける
		start = Transform(start, Multiply(ViewProjectionMatrix, ViewportMatrix));
		end = Transform(end, Multiply(ViewProjectionMatrix, ViewportMatrix));

		Novice::DrawLine((int)start.x, (int)start.y, (int)end.x, (int)end.y, 0x6F6F6FFF);
	}

	//垂直方向の線を描画
	for (uint32_t zIndex = 0; zIndex <= kSubdivision; zIndex++)
	{
		//Z軸上の座標
		float posZ = -kGridHalfWidth + kGridEvery * zIndex;

		//始点と終点
		Vector3 startZ = { -kGridHalfWidth, 0.0f, posZ };
		Vector3 endZ = { kGridHalfWidth, 0.0f, posZ };
		// ワールド座標系 -> スクリーン座標系まで変換をかける
		startZ = Transform(startZ, Multiply(ViewProjectionMatrix, ViewportMatrix));
		endZ = Transform(endZ, Multiply(ViewProjectionMatrix, ViewportMatrix));

		Novice::DrawLine((int)startZ.x, (int)startZ.y, (int)endZ.x, (int)endZ.y, 0x6F6F6FFF);
	}
}

//Sphereを表示する疑似コード
static void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color)
{
	const uint32_t kSubdivision = 20;							//分割数
	const float kLatStep = (float)M_PI / kSubdivision;			//緯度のステップ
	const float kLonStep = 2.0f * (float)M_PI / kSubdivision;	//経度のステップ

	// 緯度のループ
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex)
	{
		float lat = -0.5f * (float)M_PI + latIndex * kLatStep;	//現在の緯度

		//次の緯度
		float nextLat = lat + kLatStep;

		//経度のループ
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex)
		{
			//現在の経度
			float lon = lonIndex * kLonStep;

			//次の経度
			float nextLon = lon + kLonStep;

			// 球面座標の計算
			Vector3 pointA
			{
				sphere.center.x + sphere.radius * cos(lat) * cos(lon),
				sphere.center.y + sphere.radius * sin(lat),
				sphere.center.z + sphere.radius * cos(lat) * sin(lon)
			};

			Vector3 pointB
			{
				sphere.center.x + sphere.radius * cos(nextLat) * cos(lon),
				sphere.center.y + sphere.radius * sin(nextLat),
				sphere.center.z + sphere.radius * cos(nextLat) * sin(lon)
			};

			Vector3 pointC
			{
				sphere.center.x + sphere.radius * cos(lat) * cos(nextLon),
				sphere.center.y + sphere.radius * sin(lat),
				sphere.center.z + sphere.radius * cos(lat) * sin(nextLon)
			};

			// スクリーン座標に変換
			pointA = Transform(pointA, Multiply(viewProjectionMatrix, viewportMatrix));
			pointB = Transform(pointB, Multiply(viewProjectionMatrix, viewportMatrix));
			pointC = Transform(pointC, Multiply(viewProjectionMatrix, viewportMatrix));

			// 線分の描画
			Novice::DrawLine((int)pointA.x, (int)pointA.y, (int)pointB.x, (int)pointB.y, color);
			Novice::DrawLine((int)pointA.x, (int)pointA.y, (int)pointC.x, (int)pointC.y, color);
		}
	}
}


//AABBの描画
void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color)
{
	Vector3 vertices[8];
	vertices[0] = { aabb.min.x, aabb.min.y, aabb.min.z };
	vertices[1] = { aabb.max.x, aabb.min.y, aabb.min.z };
	vertices[2] = { aabb.min.x, aabb.max.y, aabb.min.z };
	vertices[3] = { aabb.max.x, aabb.max.y, aabb.min.z };
	vertices[4] = { aabb.min.x, aabb.min.y, aabb.max.z };
	vertices[5] = { aabb.max.x, aabb.min.y, aabb.max.z };
	vertices[6] = { aabb.min.x, aabb.max.y, aabb.max.z };
	vertices[7] = { aabb.max.x, aabb.max.y, aabb.max.z };

	for (int i = 0; i < 8; ++i)
	{
		vertices[i] = Transform(vertices[i], Multiply(viewProjectionMatrix, viewportMatrix));
	}

	Novice::DrawLine((int)vertices[0].x, (int)vertices[0].y, (int)vertices[1].x, (int)vertices[1].y, color);
	Novice::DrawLine((int)vertices[0].x, (int)vertices[0].y, (int)vertices[2].x, (int)vertices[2].y, color);
	Novice::DrawLine((int)vertices[0].x, (int)vertices[0].y, (int)vertices[4].x, (int)vertices[4].y, color);
	Novice::DrawLine((int)vertices[1].x, (int)vertices[1].y, (int)vertices[3].x, (int)vertices[3].y, color);
	Novice::DrawLine((int)vertices[1].x, (int)vertices[1].y, (int)vertices[5].x, (int)vertices[5].y, color);
	Novice::DrawLine((int)vertices[2].x, (int)vertices[2].y, (int)vertices[3].x, (int)vertices[3].y, color);
	Novice::DrawLine((int)vertices[2].x, (int)vertices[2].y, (int)vertices[6].x, (int)vertices[6].y, color);
	Novice::DrawLine((int)vertices[3].x, (int)vertices[3].y, (int)vertices[7].x, (int)vertices[7].y, color);
	Novice::DrawLine((int)vertices[4].x, (int)vertices[4].y, (int)vertices[5].x, (int)vertices[5].y, color);
	Novice::DrawLine((int)vertices[4].x, (int)vertices[4].y, (int)vertices[6].x, (int)vertices[6].y, color);
	Novice::DrawLine((int)vertices[5].x, (int)vertices[5].y, (int)vertices[7].x, (int)vertices[7].y, color);
	Novice::DrawLine((int)vertices[6].x, (int)vertices[6].y, (int)vertices[7].x, (int)vertices[7].y, color);
}

//AABBと球の当たり判定
bool IsCollision(const AABB& aabb, const Sphere& sphere)
{
	//最近接点を求める
	Vector3 clossestPoint
	{
		std::clamp(sphere.center.x,aabb.min.x,aabb.max.x),
		std::clamp(sphere.center.y,aabb.min.y,aabb.max.y),
		std::clamp(sphere.center.z,aabb.min.z,aabb.max.z)
	};
	//最近接点と球の中途の距離を求める
	float distance = Length(Subtract(clossestPoint, sphere.center));
	//距離が半径よりも小さければ衝突
	return distance <= sphere.radius;
}

const char kWindowTitle[] = "提出用課題";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	bool isDragging = false;
	int prevMouseX = 0;
	int prevMouseY = 0;

	AABB aabb{ .min{-0.5f, -0.5f, -0.5f}, .max{0.0f, 0.0f, 0.0f} };

	Vector3 rotate = {};
	Vector3 translate = {};
	Vector3 cameraTranslate = { 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate = { 0.26f, 0.0f, 0.0f };

	//球体の情報
	Sphere sphere{ {1.0f,1.0f,1.0f},{0.5f} };

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0)
	{
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		// マウス入力を取得
		POINT mousePosition;
		GetCursorPos(&mousePosition);

		///
		/// ↓更新処理ここから
		///

		  // マウスドラッグによる回転制御
		if (Novice::IsPressMouse(1))
		{
			if (!isDragging)
			{
				isDragging = true;
				prevMouseX = mousePosition.x;
				prevMouseY = mousePosition.y;
			}
			else
			{
				int deltaX = mousePosition.x - prevMouseX;
				int deltaY = mousePosition.y - prevMouseY;
				rotate.y += deltaX * 0.01f; // 水平方向の回転
				rotate.x += deltaY * 0.01f; // 垂直方向の回転
				prevMouseX = mousePosition.x;
				prevMouseY = mousePosition.y;
			}
		}
		else
		{
			isDragging = false;
		}

		// マウスホイールで前後移動
		int wheel = Novice::GetWheel();
		if (wheel != 0)
		{
			cameraTranslate.z += wheel * 0.01f; // ホイールの回転方向に応じて前後移動
		}

		ImGui::Begin("Settings");
		ImGui::DragFloat3("Camera Translate", &cameraTranslate.x, 0.01f);
		ImGui::DragFloat3("Camera Rotate", &cameraRotate.x, 0.01f);
		ImGui::DragFloat3("AABB1 Max", &aabb.max.x, 0.01f);
		ImGui::DragFloat3("AABB1 Min", &aabb.min.x, 0.01f);
		ImGui::DragFloat3("SphereCenter", &sphere.center.x, 0.01f);
		ImGui::DragFloat("SphereRadius", &sphere.radius, 0.01f);
		ImGui::End();

		// x
		aabb.min.x = (std::min)(aabb.min.x, aabb.max.x);
		aabb.max.x = (std::max)(aabb.min.x, aabb.max.x);
		// y
		aabb.min.y = (std::min)(aabb.min.y, aabb.max.y);
		aabb.max.y = (std::max)(aabb.min.y, aabb.max.y);
		// z
		aabb.min.z = (std::min)(aabb.min.z, aabb.max.z);
		aabb.max.z = (std::max)(aabb.min.z, aabb.max.z);

		//各種行列の計算
		Matrix4x4 worldMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, rotate, translate);
		Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewWorldMatrix = Inverse(worldMatrix);
		Matrix4x4 viewCameraMatrix = Inverse(cameraMatrix);

		// 透視投影行列を作成
		Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);

		//ビュー座標変換行列を作成
		Matrix4x4 viewProjectionMatrix = Multiply(viewWorldMatrix, Multiply(viewCameraMatrix, projectionMatrix));

		//ViewportMatrixビューポート変換行列を作成
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0.0f, 0.0f, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// Gridを描画
		DrawGrid(viewProjectionMatrix, viewportMatrix);

		// 衝突判定
		bool collision = IsCollision(aabb, sphere);

		DrawAABB(aabb, viewProjectionMatrix, viewportMatrix, collision ? 0xFF0000FF : 0xFFFFFFFF);
		DrawSphere(sphere, viewProjectionMatrix, viewportMatrix, 0xFFFFFFFF);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0)
		{
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
