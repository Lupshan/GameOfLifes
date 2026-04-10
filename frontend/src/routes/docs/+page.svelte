<svelte:head>
	<title>Language Docs - GameOfLifes</title>
</svelte:head>

<div class="container docs">
	<h1>GoL Script Language Reference</h1>

	<section class="card">
		<h2>Overview</h2>
		<p>
			GoL Script is a minimal scripting language for writing bot programs. It compiles to bytecode
			executed by the simulation engine VM.
		</p>
	</section>

	<section class="card">
		<h2>Types</h2>
		<p>
			<code>int</code> (32-bit signed) — the only type. Booleans are ints: <code>true</code> = 1,
			<code>false</code> = 0.
		</p>
	</section>

	<section class="card">
		<h2>Functions</h2>
		<pre>fn main() &#123;
    // Entry point, called every tick.
&#125;

fn helper(a, b) &#123;
    return a + b;
&#125;</pre>
	</section>

	<section class="card">
		<h2>Variables</h2>
		<pre>let x = 42;      // declare
x = x + 1;       // assign
// Variables persist across ticks (agent memory).</pre>
	</section>

	<section class="card">
		<h2>Control Flow</h2>
		<pre>if (condition) &#123;
    // ...
&#125; else &#123;
    // ...
&#125;

while (condition) &#123;
    // ...
&#125;</pre>
	</section>

	<section class="card">
		<h2>Operators</h2>
		<p>Arithmetic: <code>+ - * / %</code></p>
		<p>Comparison: <code>== != &lt; &gt; &lt;= &gt;=</code></p>
		<p>Logic: <code>&amp;&amp; || !</code></p>
	</section>

	<section class="card">
		<h2>Builtin Functions</h2>
		<div class="builtin-group">
			<h3>Actions</h3>
			<ul>
				<li><code>move()</code> — move forward</li>
				<li><code>turn_left()</code> / <code>turn_right()</code> — rotate 90°</li>
				<li><code>eat()</code> → energy gained</li>
				<li><code>drink()</code> → hydration gained</li>
				<li><code>reproduce()</code> → 1 on success, 0 on failure</li>
			</ul>
		</div>
		<div class="builtin-group">
			<h3>Perception</h3>
			<ul>
				<li>
					<code>perceive_food()</code> / <code>perceive_water()</code> /
					<code>perceive_terrain()</code> — current cell
				</li>
				<li><code>look_food()</code> / <code>look_water()</code> — cell ahead</li>
				<li><code>my_energy()</code> / <code>my_hydration()</code></li>
				<li>
					<code>my_trait(id)</code> — 0=vision, 1=speed, 2=metabolism, 3=repro_threshold, 4=swim, 5=forage
				</li>
			</ul>
		</div>
	</section>

	<section class="card">
		<h2>Example: Food Seeker</h2>
		<pre>fn main() &#123;
    let food = look_food();
    if (food > 0) &#123;
        move();
        eat();
    &#125; else &#123;
        turn_right();
    &#125;
    drink();
    if (my_energy() > 100) &#123;
        reproduce();
    &#125;
&#125;</pre>
	</section>
</div>

<style>
	.docs {
		display: flex;
		flex-direction: column;
		gap: var(--sp-4);
	}

	.docs h1 {
		margin-bottom: var(--sp-2);
	}

	.docs section h2 {
		font-size: var(--text-xl);
		margin-bottom: var(--sp-3);
		padding-bottom: var(--sp-2);
		border-bottom: 1px solid var(--border);
	}

	.docs section p:last-child {
		margin-bottom: 0;
	}

	.docs pre {
		margin-bottom: 0;
	}

	.builtin-group {
		margin-bottom: var(--sp-4);
	}

	.builtin-group:last-child {
		margin-bottom: 0;
	}

	.builtin-group h3 {
		font-size: var(--text-base);
		color: var(--accent);
		margin-bottom: var(--sp-2);
	}

	.builtin-group ul {
		margin-bottom: 0;
	}

	.builtin-group li {
		font-size: var(--text-sm);
	}
</style>
