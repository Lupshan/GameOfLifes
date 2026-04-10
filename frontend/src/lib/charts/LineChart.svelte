<script lang="ts">
	interface DataPoint {
		x: number;
		y: number;
	}

	interface Series {
		label: string;
		color: string;
		data: DataPoint[];
	}

	let { series, width = 600, height = 300 }: { series: Series[]; width?: number; height?: number } = $props();

	const padding = { top: 20, right: 20, bottom: 30, left: 50 };

	function getPath(data: DataPoint[], xMin: number, xMax: number, yMax: number): string {
		if (data.length === 0) return '';
		const w = width - padding.left - padding.right;
		const h = height - padding.top - padding.bottom;
		const xRange = xMax - xMin || 1;

		return data
			.map((p, i) => {
				const x = padding.left + ((p.x - xMin) / xRange) * w;
				const y = padding.top + h - (p.y / (yMax || 1)) * h;
				return `${i === 0 ? 'M' : 'L'}${x},${y}`;
			})
			.join(' ');
	}

	let allPoints = $derived(series.flatMap((s) => s.data));
	let xMin = $derived(Math.min(...allPoints.map((p) => p.x), 0));
	let xMax = $derived(Math.max(...allPoints.map((p) => p.x), 1));
	let yMax = $derived(Math.max(...allPoints.map((p) => p.y), 1));
</script>

<svg {width} {height} style="background:#fafafa;border:1px solid #ddd;">
	<!-- Y axis -->
	<line x1={padding.left} y1={padding.top} x2={padding.left} y2={height - padding.bottom} stroke="#999" />
	<!-- X axis -->
	<line x1={padding.left} y1={height - padding.bottom} x2={width - padding.right} y2={height - padding.bottom} stroke="#999" />

	<!-- Y label -->
	<text x={10} y={padding.top + 5} font-size="11" fill="#666">{yMax}</text>
	<text x={10} y={height - padding.bottom} font-size="11" fill="#666">0</text>

	<!-- X labels -->
	<text x={padding.left} y={height - 5} font-size="11" fill="#666">{xMin}</text>
	<text x={width - padding.right} y={height - 5} font-size="11" fill="#666" text-anchor="end">{xMax}</text>

	<!-- Series -->
	{#each series as s}
		<path d={getPath(s.data, xMin, xMax, yMax)} fill="none" stroke={s.color} stroke-width="2" />
	{/each}

	<!-- Legend -->
	{#each series as s, i}
		<rect x={padding.left + i * 100} y={2} width={12} height={12} fill={s.color} />
		<text x={padding.left + i * 100 + 16} y={12} font-size="11" fill="#333">{s.label}</text>
	{/each}
</svg>
