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
	float Depth : TEXCOORD0;
};

DEPTH_MAP_OUT VS_DepthMap(VERTEX_IN v, uniform float4x4 worldViewProj, uniform float4x4 worldView)
{
	DEPTH_MAP_OUT res;
	res.Position = mul(v.Position, worldViewProj);
	float4 camPos = mul(v.Position, worldView);
	res.Depth = -camPos.z;
	return res;
}

float4 PS_DepthMap(DEPTH_MAP_OUT p) : COLOR
{
	//return float4(1,0,0,1);
	

	float d = p.Depth;
		
	return float4(d,d,d,1);
}

uniform sampler2D original : register(s0);
uniform sampler2D originalNoGround : register(s1);
uniform sampler2D toonshaded : register(s2);
uniform float4 viewportSize;

float4 samp(float2 p, float dx, float dy, float s)
{
	const float S = 1.0;

	float2 d = float2(dx, dy);
	p = p+S*viewportSize.zw*d;

	float4 c = tex2D(originalNoGround, p)*s;
	return c;
}


float4 MiniWindow(float4 c)
{
	return c;
	
}


float Fog(float c)
{
	const float FOG_START = 10000;
	const float FOG_END = 100000;
	const float FOG_EXPONENT = 0.2;
	if (c == 0)
	{
		return 1;
	}
	else
	{
		c = saturate((c-FOG_START)/(FOG_END-FOG_START));
		return pow(c, FOG_EXPONENT);
	}
}

float4 PS_Compose(float2 p : TEXCOORD0, uniform float4 fogColor) : COLOR
{
	if (p.x>0.75 && p.y>0.75)
	{
		p-=float2(0.75, 0.75);
		p*=4;
		float4 c = tex2D(originalNoGround, p);
		return MiniWindow(c);		
	}
	else
	{
		float depth = tex2D(originalNoGround, p).a;
		
		float4 Gx = samp(p,-1,-1,-1)+samp(p,1,-1,1) +
				samp(p,-1,0,-2)+samp(p,1,0,2) +
				samp(p,-1,1,-1)+samp(p,1,1,1);
	
		float4 Gy = samp(p,-1,-1,-1)+samp(p,0,-1,-2)+samp(p,1,-1,-1)+
				samp(p,-1,1,1)+samp(p,0,1,2)+samp(p,1,1,1);
		float4 G = Gx*Gx+Gy*Gy;
		
		G.a /= pow(depth, 2.2);
		G.a = pow(G.a, 0.6);
		G.a = saturate(G.a);
		//float bw = G.a*4;
		//return (float4)bw;
		float bw = G.r * 0.21f + G.g * 0.39f + G.b * 0.4f + G.a*2;
		

		//bw /= 4;
		bw = saturate(bw);
		bw = 1-bw;

		

		float4 pix = tex2D(toonshaded, p);
		pix.rgb*=bw;

		//return pix;

		fogColor = float4(0.8, 0.8, 0.8, 1);

		float fog = Fog(depth);
		return lerp(pix, fogColor, fog);

		//return pix;
	}
}
