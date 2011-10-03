#pragma once

namespace	LibESGL
{
	class	Vertex
	{
		public:
			void				Set			(float aX, float aY, float aU, float aV, float aR, float aG, float aB, float aA)
			{
				X = aX; Y = aY; Z = 0.0f; U = aU; V = aV; R = aR; G = aG; B = aB; A = aA;
			}

			void				Set			(float aX, float aY, float aU, float aV, uint32_t aRGBA)
			{
				X = aX; Y = aY; Z = 0.0f; U = aU; V = aV;
				R = ((aRGBA >> 24) & 0xFF) / 255.0f;
				G = ((aRGBA >> 16) & 0xFF) / 255.0f;
				B = ((aRGBA >>  8) & 0xFF) / 255.0f;
				A = ((aRGBA >>  0) & 0xFF) / 255.0f;
			}

			static uint32_t		Size		()
			{
				return 9 * sizeof(float);
			}


			float	X, Y, Z, U, V, R, G, B, A;
	};
}

