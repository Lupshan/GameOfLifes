<svelte:head>
	<title>Language Docs - GameOfLifes</title>
</svelte:head>

<div style="max-width:800px;margin:2rem auto;padding:1rem;font-family:monospace;">
	<h1>GoL Script Language Reference</h1>

	<h2>Overview</h2>
	<p>GoL Script is a minimal scripting language for writing bot programs. It compiles to bytecode executed by the simulation engine VM.</p>

	<h2>Types</h2>
	<p><code>int</code> (32-bit signed) — the only type. Booleans are ints: <code>true</code> = 1, <code>false</code> = 0.</p>

	<h2>Functions</h2>
	<pre>fn main() &#123;
    // Entry point, called every tick.
&#125;

fn helper(a, b) &#123;
    return a + b;
&#125;</pre>

	<h2>Variables</h2>
	<pre>let x = 42;      // declare
x = x + 1;       // assign
// Variables persist across ticks (agent memory).</pre>

	<h2>Control Flow</h2>
	<pre>if (condition) &#123;
    // ...
&#125; else &#123;
    // ...
&#125;

while (condition) &#123;
    // ...
&#125;</pre>

	<h2>Operators</h2>
	<p>Arithmetic: <code>+ - * / %</code></p>
	<p>Comparison: <code>== != &lt; &gt; &lt;= &gt;=</code></p>
	<p>Logic: <code>&amp;&amp; || !</code></p>

	<h2>Builtin Functions</h2>
	<h3>Actions</h3>
	<ul>
		<li><code>move()</code> — move forward</li>
		<li><code>turn_left()</code> / <code>turn_right()</code> — rotate 90°</li>
		<li><code>eat()</code> → energy gained</li>
		<li><code>drink()</code> → hydration gained</li>
		<li><code>reproduce()</code> → 1 on success, 0 on failure</li>
	</ul>
	<h3>Perception</h3>
	<ul>
		<li><code>perceive_food()</code> / <code>perceive_water()</code> / <code>perceive_terrain()</code> — current cell</li>
		<li><code>look_food()</code> / <code>look_water()</code> — cell ahead</li>
		<li><code>my_energy()</code> / <code>my_hydration()</code></li>
		<li><code>my_trait(id)</code> — 0=vision, 1=speed, 2=metabolism, 3=repro_threshold, 4=swim, 5=forage</li>
	</ul>

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
</div>
