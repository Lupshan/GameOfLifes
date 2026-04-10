<svelte:head>
	<title>GameOfLifes — A Living Ecosystem</title>
</svelte:head>

<div class="hero">
	<div class="hero-content">
		<h1 class="hero-title">Game<span class="accent">Of</span>Lifes</h1>
		<p class="hero-subtitle">
			A web platform where user-submitted AIs drive agents in a shared, persistent simulated world.
			Watch evolution unfold.
		</p>

		<div class="hero-actions">
			<a href="/world" class="btn btn-primary btn-lg">Watch the World</a>
			<a href="/edit" class="btn btn-lg">Create a Bot</a>
		</div>

		<div class="features">
			<div class="feature">
				<span class="feature-icon">&#x1F9EC;</span>
				<h3>Genetics & Evolution</h3>
				<p>
					Agents carry genomes with heritable traits. Mutations drive natural selection across
					generations.
				</p>
			</div>
			<div class="feature">
				<span class="feature-icon">&#x1F310;</span>
				<h3>Persistent World</h3>
				<p>
					A shared toroidal grid with dynamic seasons, multiple resources, and environmental cycles.
				</p>
			</div>
			<div class="feature">
				<span class="feature-icon">&#x1F916;</span>
				<h3>Code Your AI</h3>
				<p>
					Write bot logic with a visual block editor or scripting language. Compiled to bytecode,
					executed in a sandboxed VM.
				</p>
			</div>
		</div>
	</div>
</div>

<style>
	.hero {
		display: flex;
		justify-content: center;
		align-items: center;
		min-height: calc(100vh - var(--nav-height));
		padding: var(--sp-8) var(--sp-4);
	}

	.hero-content {
		max-width: 720px;
		text-align: center;
	}

	.hero-title {
		font-size: 3.5rem;
		font-weight: 700;
		letter-spacing: -0.03em;
		margin-bottom: var(--sp-4);
	}

	.accent {
		color: var(--accent);
	}

	.hero-subtitle {
		font-size: var(--text-lg);
		color: var(--text-secondary);
		line-height: 1.7;
		margin-bottom: var(--sp-8);
		max-width: 540px;
		margin-left: auto;
		margin-right: auto;
	}

	.hero-actions {
		display: flex;
		gap: var(--sp-3);
		justify-content: center;
		margin-bottom: var(--sp-16);
	}

	.btn-lg {
		padding: var(--sp-3) var(--sp-8);
		font-size: var(--text-base);
		font-weight: 600;
		border-radius: var(--radius-lg);
		text-decoration: none;
	}

	.features {
		display: grid;
		grid-template-columns: repeat(3, 1fr);
		gap: var(--sp-6);
		text-align: left;
	}

	.feature {
		background: var(--bg-surface);
		border: 1px solid var(--border);
		border-radius: var(--radius-lg);
		padding: var(--sp-6);
		transition:
			border-color var(--transition),
			box-shadow var(--transition);
	}

	.feature:hover {
		border-color: var(--border-strong);
		box-shadow: var(--shadow);
	}

	.feature-icon {
		font-size: 2rem;
		display: block;
		margin-bottom: var(--sp-3);
	}

	.feature h3 {
		font-size: var(--text-base);
		margin-bottom: var(--sp-2);
	}

	.feature p {
		font-size: var(--text-sm);
		margin-bottom: 0;
	}

	@media (max-width: 768px) {
		.hero-title {
			font-size: 2.5rem;
		}

		.hero-actions {
			flex-direction: column;
			align-items: center;
		}

		.features {
			grid-template-columns: 1fr;
		}
	}
</style>
