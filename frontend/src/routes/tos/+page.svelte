<svelte:head>
	<title>Terms of Service - GameOfLifes</title>
</svelte:head>

<div class="container">
	<h1>Terms of Service</h1>
	<p class="updated"><em>Last updated: April 2026</em></p>

	<div class="card tos-content">
		<h2>1. Acceptance</h2>
		<p>By using GameOfLifes, you agree to these terms.</p>

		<h2>2. Account</h2>
		<p>You are responsible for your account credentials. One account per person.</p>

		<h2>3. Bots</h2>
		<p>
			Submitted bots are executed in a sandboxed environment. You may not submit code designed to
			exploit the platform infrastructure.
		</p>

		<h2>4. Fair use</h2>
		<p>
			Quotas apply: max 10 bots per user, max 10 KB source per bot. Abuse leads to account
			suspension.
		</p>

		<h2>5. Content</h2>
		<p>
			Bot source code you submit remains yours. The platform may display bot names and statistics
			publicly.
		</p>

		<h2>6. No warranty</h2>
		<p>The platform is provided as-is. We do not guarantee uptime or data persistence.</p>

		<h2>Privacy Policy</h2>
		<p>
			We store your email and hashed password. Bot source code and simulation data are stored for
			platform operation. No data is sold to third parties.
		</p>
	</div>
</div>

<style>
	.updated {
		color: var(--text-muted);
		margin-bottom: var(--sp-6);
	}

	.tos-content h2 {
		font-size: var(--text-lg);
		margin-top: var(--sp-6);
		margin-bottom: var(--sp-2);
	}

	.tos-content h2:first-child {
		margin-top: 0;
	}

	.tos-content p:last-child {
		margin-bottom: 0;
	}
</style>
