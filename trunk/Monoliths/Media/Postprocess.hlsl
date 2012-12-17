struct VERTEX_IN 
{ 
	float4 Position : POSITION;
	float4 Normal : NORMAL;
	float2 Texture : TEXCOORD0;
};


struct VERTEX_OUT
{
	float4 Position : POSITION;
	float2 Texture : TEXCOORD0;
};

VERTEX_OUT VS_Quad(VERTEX_IN v, uniform float4x4 worldViewProj)
{
	VERTEX_OUT res;
	res.Position = mul(v.Position, worldViewProj); 
	res.Texture = v.Texture;
	return res;
}

struct DEPTH_MAP_OUT
{
	float4 Position : POSITION;
	float2 DepthHeight : TEXCOORD0;
};

DEPTH_MAP_OUT VS_DepthMap(VERTEX_IN v, uniform float4x4 worldViewProj, uniform float4x4 worldView, uniform float4x4 world)
{
	DEPTH_MAP_OUT res;
	res.Position = mul(v.Position, worldViewProj);
	float4 camPos = mul(v.Position, worldView);
	res.DepthHeight.x = -camPos.z;
	res.DepthHeight.y = mul(v.Position, world).y;
	return res;
}

float4 PS_DepthMap(DEPTH_MAP_OUT p, uniform float objectId) : COLOR
{
	float2 d = p.DepthHeight;		
	return float4(d.x,d.y,objectId,0);
}

uniform sampler2D original : register(s0);
uniform sampler2D depthmap : register(s1);
uniform sampler2D toonshaded : register(s2);
uniform sampler2D contour : register(s3);

uniform float4 viewportSize;

float4 samp(float2 p, float dx, float dy, float s)
{
	//float depth = tex2D(depthmap, p).x;
	const float S = 1.0;

	float2 d = float2(dx, dy);
	p = p+S*viewportSize.zw*d;

	float4 c;
	c.rgb = tex2D(original, p).rgb*s;
	c.a = tex2D(depthmap, p).r*s;
	return c;
}


float4 MiniWindow(float4 c)
{
	return c;
}

float Fog(float2 dh)
{
	const float2 FOG_START = float2(3000, 800);
	const float2 FOG_END = float2(35000, 8000);
	const float2 FOG_EXPONENT = float2(0.7, 0.9);
	if (dh.x == 0)
	{
		return 1;
	}
	else
	{
		dh = saturate((dh-FOG_START)/(FOG_END-FOG_START));
		dh = pow(dh, FOG_EXPONENT);
		//return length(dh);
		return saturate(dh.x+dh.y);
	}
}

float GetId(float2 p)
{
	return tex2D(depthmap, p).b;
}

bool IsObjContour(float2 p)
{
	const float S = 1.0;
	const float2 Kernel[8] = 
	{
		{-1,-1}, {0,-1}, {1,-1},
		{-1,0},          {1,0},
		{-1,1}, {0,1}, {1,1},
	};

	float id = GetId(p);
	int res = 0;
	for (int i=0;i<8;i++)
	{
		if (GetId(p+Kernel[i]*viewportSize.zw*S) != id) return true;
	}
	return false;
}

float4 PS_Contour(float2 p : TEXCOORD0) : COLOR
{
	float2 dh = tex2D(depthmap, p).rg;
	
	float4 Gx = samp(p,-1,-1,-1)+samp(p,1,-1,1) +
			samp(p,-1,0,-2)+samp(p,1,0,2) +
			samp(p,-1,1,-1)+samp(p,1,1,1);
	
	float4 Gy = samp(p,-1,-1,-1)+samp(p,0,-1,-2)+samp(p,1,-1,-1)+
			samp(p,-1,1,1)+samp(p,0,1,2)+samp(p,1,1,1);
	float4 G = Gx*Gx+Gy*Gy;
		
	if (!IsObjContour(p))
	{
		G.a /= pow(dh.x, 2.5);
	}
	//G.a /= pow(dh.x, 1.8);
	G.a = 2*pow(G.a, 0.6);

	//G.rgb /=pow(dh.x*0.0001, 0.3);
	G.rgb*=100;
	float bw = G.r * 0.21f + G.g * 0.39f + G.b * 0.4f + G.a;

	return saturate(bw);
}

float4 BlurContour(float2 p)
{
	const float S = 0.5;
	const float2 PixelKernel[9] = 
	{
		{-1,-1}, {0,-1}, {1,-1},
		{-1,0}, {0,0}, {1,0},
		{-1,1}, {0,1}, {1,1},
	};

	float BlurWeights[9] = 
	{
		0.1, 0.1, 0.1,
		0.1, 0.4, 0.1,
		0.1, 0.1, 0.1
	};

	float4 c = (float4)0;
	for (int i=0;i<9;i++)
	{
		c+= tex2D(contour, p+S*PixelKernel[i]*viewportSize.zw)*BlurWeights[i];
	}
	return saturate(c);
}

float2 NearestDHAroundPixel(float2 p)
{
	const float2 PixelKernel[9] = 
	{
		{-1,-1}, {0,-1}, {1,-1},
		{-1,0}, {0,0}, {1,0},
		{-1,1}, {0,1}, {1,1},
	};

	
	float2 texSize = float2(1 / viewportSize.x, 1/viewportSize.y);
	float2 nearestDH = float2(100000,0);
	for (int i=0;i<9;i++)
	{
		float2 val = tex2D(depthmap, p+PixelKernel[i]*texSize).xy; // xy == depth,height (DH)
		if (val.x != 0 && val.x<nearestDH.x) nearestDH = val;
	}
	return nearestDH;
}

float4 PS_Compose(float2 p : TEXCOORD0) : COLOR
{
	static const float4 fogColor = float4(0.8, 0.8, 0.8, 1);

	if (p.x>0.75 && p.y>0.75)
	{
		p-=float2(0.75, 0.75);
		p*=4;
		float4 c = tex2D(original, p);
		return MiniWindow(c);		
	}
	else
	{

		float bw = 1-BlurContour(p).x;
		//return bw;

		float4 pix = tex2D(toonshaded, p);
		pix.rgb*=bw;
		
		float2 dh = NearestDHAroundPixel(p);
		float fog = Fog(dh);
		return lerp(pix, fogColor, fog);
	}
}
