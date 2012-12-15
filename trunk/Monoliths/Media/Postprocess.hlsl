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

float4 PS_DepthMap(DEPTH_MAP_OUT p) : COLOR
{
	float2 d = p.DepthHeight;		
	return float4(d.x,d.y,0,0);
}

uniform sampler2D original : register(s0);
uniform sampler2D toonshaded : register(s1);
uniform sampler2D depthmap : register(s2);
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
	const float2 FOG_END = float2(35000, 6000);
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

float4 PS_Compose(float2 p : TEXCOORD0, uniform float4 fogColor) : COLOR
{
	if (p.x>0.75 && p.y>0.75)
	{
		p-=float2(0.75, 0.75);
		p*=4;
		float4 c = tex2D(original, p);
		return MiniWindow(c);		
	}
	else
	{
		//float depth = tex2D(original, p).a;
		float2 dh = tex2D(depthmap, p).rg;
		
		float4 Gx = samp(p,-1,-1,-1)+samp(p,1,-1,1) +
				samp(p,-1,0,-2)+samp(p,1,0,2) +
				samp(p,-1,1,-1)+samp(p,1,1,1);
	
		float4 Gy = samp(p,-1,-1,-1)+samp(p,0,-1,-2)+samp(p,1,-1,-1)+
				samp(p,-1,1,1)+samp(p,0,1,2)+samp(p,1,1,1);
		float4 G = Gx*Gx+Gy*Gy;
		
		G.a /= pow(dh.x, 2.1);
		G.a = pow(G.a, 0.6);
		G.a = saturate(G.a);
		//float bw = G.a*4;
		//return (float4)bw;
		

		//G.rgb /=pow(dh.x*0.002, 1.05);
		float bw = G.r * 0.21f + G.g * 0.39f + G.b * 0.4f + G.a*2;
		bw /= 4;
		
		bw = 1-saturate(bw);

		float4 pix = tex2D(toonshaded, p);
		pix.rgb*=bw;

		//return pix;

		fogColor = float4(0.8, 0.8, 0.8, 1);

		float fog = Fog(dh);
		return lerp(pix, fogColor, fog);

		//return pix;
	}
}
