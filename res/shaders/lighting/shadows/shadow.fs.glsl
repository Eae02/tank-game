in float depth_v;

layout(location=0) out float depth_out;

void main()
{
	depth_out = min(depth_v, 1.0);
}
