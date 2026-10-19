// context.c - background for every question, shown by the Context button in battle.
// CONTEXT[r][i] belongs to REGIONS[r].qs[i]: it explains the ideas and terms the
// question uses, and when you would reach for them in real work.
// Plain ASCII only: the bundled font is an ASCII subset.
#include "content.h"

const char *const CONTEXT[NREGIONS][NQ] = {

// ================================================================ 1 Prompt craft
{
    "A prompt is everything you send Claude: the request plus any background and material. Claude is very capable, but each new conversation starts blank. It has never seen your project, your audience or your goals.\n\n"
    "So the question is really: what kind of reader should you write for? Think about how much you would have to explain to someone smart who just walked in the door.",

    "Executives reading a summary want to make a decision, not read everything. A useful prompt names four things: the format (bullets, length), the audience (who reads it), the purpose (what decision it supports) and the focus (what matters most).\n\n"
    "Compare the options by asking: which one would let a stranger write the summary without asking you a single question?",

    "An instruction is a rule (\"keep it under 100 words\"). A reason is the why behind it (\"it goes on a phone lock screen\").\n\n"
    "Claude reads the whole prompt and tries to serve your goal. When it knows the goal, it can make good choices in situations your rules never mentioned, the way a colleague who understands the point needs fewer rules.",

    "XML tags are labels you wrap around part of a prompt, written like <contract> ... </contract>. Use them when a prompt mixes your instructions with material Claude should work on, such as a contract, an email or a data table.\n\n"
    "Example: paste the text between <contract> and </contract>, then write \"List every termination clause in the contract in <contract> tags.\" The tag name is yours to choose; there is no special list. Pick a name that describes the content and use it consistently.",

    "Few-shot (or multishot) prompting means including a few worked examples of input and ideal output in your prompt, so Claude can see the pattern you want instead of guessing it from a description.\n\n"
    "Examples are powerful for format and tone. Two things to weigh: how many you need to show a pattern, and how much they should vary so Claude learns the pattern rather than one specific case.",

    "When you give examples, Claude treats everything they have in common as part of the pattern to follow. It cannot tell an intended feature (\"use three bullets\") from an accident (\"every one happens to open the same way\").\n\n"
    "Before you add examples, read them side by side and ask: what do they all share that I did not mean to teach?",

    "A token is a chunk of text, roughly three quarters of a word, so 30,000 tokens is a long stack of documents. With long material, the order of the parts of a prompt matters.\n\n"
    "The usual layout for long-context prompts: reference material first, wrapped in tags, then your instructions and question. Anthropic's testing found that one ordering gives noticeably better answers.",

    "Markdown is the formatting syntax behind headings (#), bullets (-) and bold (**text**). It looks great in a chat window but shows up as stray symbols when the text is pasted into an email or a plain-text field.\n\n"
    "A general prompting principle applies here: Claude responds better to a description of what you want than to a ban on what you don't. Your prompt's own style also rubs off on the reply.",

    "Some jobs have several distinct stages, such as research, then outline, then draft, then edit. Doing them all in one prompt splits Claude's attention, and when the result is weak you cannot tell which stage failed.\n\n"
    "Prompt chaining means running each stage as its own prompt and feeding one step's output into the next. You can check and fix the work between steps.",

    "An API request is the structured message your code sends to Claude. It has several parts: the conversation messages, settings such as max_tokens, and a separate field for standing instructions that apply to the whole conversation.\n\n"
    "A role (\"You are a senior SOC analyst\") is a standing instruction: it shapes every reply, not just one, so it belongs with the conversation-wide setup.",

    "Hard problems (multi-step logic, math, planning) go better when Claude works through the reasoning before it commits to an answer, just as a person does better with scratch paper.\n\n"
    "You can ask for that in the prompt (\"think it through step by step\"), or turn on extended thinking, which gives Claude a dedicated reasoning phase before its final reply.",

    "When your code reads Claude's reply, it needs to find the answer reliably every time. Free-form prose is hard to parse: the verdict might be in the first sentence or the last.\n\n"
    "You can ask Claude to put specific parts of its output in a predictable place, such as inside named tags or a JSON object with fields you define. Then your code just extracts that part.",
},

// ================================================================ 2 The Claude apps
{
    "Claude comes in model tiers. Opus is the most capable, for the hardest reasoning and agentic work. Sonnet balances intelligence and speed and is a good everyday default. Haiku is the smallest, fastest and cheapest.\n\n"
    "Picking a tier is a trade-off: use the smallest model that does the job well, and save the big one for tasks that need it.",

    "A Project is a workspace in the Claude apps for one ongoing effort. It holds two kinds of shared setup: project knowledge (files Claude can draw on, such as PDFs and notes) and project instructions (how Claude should behave in that project).\n\n"
    "Every chat started inside the Project gets both automatically, so you set them up once instead of repeating yourself.",

    "Claude does not remember a conversation between messages the way a person does. Each time you send a message, the whole conversation so far (your messages, its replies, any files) is sent and read again to produce the next reply.\n\n"
    "Usage limits are based on how much text Claude processes. Think about what that means for message 80 of a very long chat.",

    "In the Claude apps you can edit a message you sent earlier. Claude then answers the edited version, starting a new branch of the conversation from that point, and the later messages are set aside (you can switch back to them).\n\n"
    "When a chat goes wrong, the bad turns stay in context and keep influencing replies. Removing them is often better than arguing with them.",

    "Styles control how Claude writes: tone, length and formatting. There are built-in presets (such as Concise or Formal), and you can create a custom Style by giving Claude samples of your own writing to match.\n\n"
    "A Style applies to any chat where you select it, so it fits a voice you want to reuse everywhere.",

    "Incognito chats are conversations that are not saved to your chat history and are not used for Claude's memory of you.\n\n"
    "They suit sensitive or one-off topics you do not want showing up later, such as a medical worry, a surprise gift or a draft resignation letter.",

    "Web search lets Claude look things up and cite sources for a quick question. Research is a bigger mode: Claude plans and runs many searches on its own over several minutes, across the web and any sources you have connected, then writes a structured report with citations.\n\n"
    "Use quick search for one fact. Use Research for a question that needs dozens of sources pulled together.",

    "Claude's built-in knowledge comes from its training data, which stops at a cutoff date. It has no way to know about events after that point.\n\n"
    "To answer questions about recent events, Claude needs a tool that fetches current information from the internet, and it should show where each fact came from so you can check it.",

    "Out of the box, a chat model only produces text. With code execution and file creation turned on, Claude can write and run code in a sandbox, and use that to produce real files: spreadsheets, Word documents, slide decks, PDFs and charts you can download.\n\n"
    "This is how Claude can build a working Excel model rather than describe one.",

    "The Claude apps have settings at different levels. Profile preferences (in Settings) are about you and apply to every chat: your role, preferred units, how much detail you like. Project instructions only apply inside one Project. A message only affects that one chat.\n\n"
    "Pick the level that matches how widely the preference should apply.",

    "A Project has two separate boxes. Project instructions are directions: \"You are my thesis advisor; cite in APA; be blunt about weak arguments.\" Project knowledge is material: the PDFs, notes and data Claude can consult when relevant.\n\n"
    "A useful way to tell them apart: one tells Claude how to act, the other gives it things to read.",

    "Extended thinking gives Claude a dedicated reasoning phase before it answers: it works through the problem, checks steps and considers alternatives, and you can usually view that thinking.\n\n"
    "It costs more time and usage, so it is worth turning on for puzzles, planning, tricky math and complex analysis, not for simple questions.",
},

// ================================================================ 3 Artifacts
{
    "An artifact is substantial, self-contained content that Claude creates in a separate panel beside the chat, rather than inside the message stream. Typical artifacts are documents, code, diagrams, SVG images and small interactive apps.\n\n"
    "Claude makes an artifact when the content is big enough to stand alone and is something you will likely reuse, edit or share.",

    "Artifacts render in the browser. Common types: Markdown documents, code files, single-page HTML, SVG graphics, Mermaid diagrams and React components that run as live interactive apps.\n\n"
    "Everything an artifact can be has to display or run inside a web page. Think about which kinds of output a browser panel cannot run.",

    "Each time Claude updates an artifact, the earlier versions are kept. You can step back through them with the version controls in the artifact panel and continue from any one.\n\n"
    "This is the artifact equivalent of undo: when a change makes things worse, you do not have to describe the old state to get it back.",

    "Iterating means improving an artifact through several rounds of feedback. Each request can either rebuild everything or change one specific thing.\n\n"
    "A focused request (\"make the header sticky, keep everything else\") keeps the parts that already work and makes it easy to see whether the change did what you wanted.",

    "You can publish an artifact and share a link. People who open it can view and use it. They can also remix it: that makes their own copy in their own conversation, which they can change freely without touching your original.\n\n"
    "Sharing is read-only for viewers; remixing is how someone gets an editable version.",

    "An AI-powered artifact is an app that calls Claude itself while it runs, for example a quiz generator or a writing coach.\n\n"
    "When someone else uses your shared AI-powered artifact, the Claude calls it makes are counted against the account of the person using it, not the person who built it. That is what lets you share such apps widely without paying for everyone.",

    "A sandbox is a walled-off environment that limits what code inside it can reach. Artifacts run in one, so a shared artifact cannot quietly send your data to an unknown server or load untrusted code.\n\n"
    "As a result, artifacts can only make outside network requests to a small set of allowed services, and calling an arbitrary website fails.",

    "Mermaid is a text-based language for diagrams. You write simple lines such as \"Detect --> Triage --> Contain\" and it draws a flowchart, sequence diagram, timeline or similar chart.\n\n"
    "Because the diagram is text, Claude can produce and edit it easily, and you can paste it into tools that support Mermaid, such as GitHub and many wikis.",

    "Code can appear two ways: as a code block inside the chat message, or as an artifact in its own panel with versions and a preview.\n\n"
    "The artifact panel is worth it for something you will run, extend or revisit. For a quick one-off, like a single command or a three-line example, opening a separate panel adds overhead.",

    "React is a popular JavaScript library for building user interfaces from components. A React artifact is a component that Claude writes and the artifact panel runs immediately.\n\n"
    "That means you get working buttons, inputs, charts and state right there next to the conversation, not a description of an app.",

    "Promptwood's lesson: Claude starts with no context, so the best prompts state the goal, the audience and what finished looks like.\n\n"
    "For an app or dashboard, \"finished\" includes what it is for, who will use it, what data it shows, which features matter and any design limits (colors, layout, screen size). Each missing piece is something Claude has to guess.",

    "Chat replies scroll away as the conversation continues and are awkward to share. An artifact is a separate, persistent piece of content with its own panel: you can copy it, download it, publish it as a link and keep improving it.\n\n"
    "Asking for one is the way to turn an answer into a thing you keep.",
},

// ================================================================ 4 Connectors and MCP
{
    "MCP is the open standard Anthropic introduced for connecting AI applications to outside tools and data. Before it, every app needed a custom integration for every service.\n\n"
    "The name describes what it does: a shared set of rules (a protocol) for giving a model the context it needs from other systems.",

    "An MCP server is a small program that exposes something to Claude, such as GitHub, a database or your calendar. The protocol defines what a server can offer.\n\n"
    "One kind is actions Claude can call (create an issue). Another is data Claude can read (a file, a record). A third is reusable prompt templates the user can pick. Each has its own name in the spec.",

    "Inside MCP, the two main things a server offers are tools and resources.\n\n"
    "Think of the difference between a verb and a noun: \"send_message\" or \"create_ticket\" does something in the world, while \"the contents of report.pdf\" or \"customer record 42\" is something to look at.",

    "An open standard is a published specification that anyone can implement for free, like HTTP or USB. No single company controls who can use it.\n\n"
    "For integrations that matters a lot: if a server follows the standard, it can plug into any app that also follows it, instead of being rebuilt for each one.",

    "Claude often reads untrusted text: emails, web pages, documents. Prompt injection is when that text contains instructions written to hijack the AI, hoping Claude will obey them as if they came from you.\n\n"
    "The defense is a mindset: content Claude is reading is data to analyze, and only you (the user) give instructions. Suspicious instructions inside data are worth flagging.",

    "Each connector you enable adds its tool definitions (names, descriptions and parameters) to Claude's context, so Claude knows what it can call.\n\n"
    "Context is limited, and a long menu of similar tools makes it harder for Claude to pick the right one. Connectors you are not using still take up space.",

    "Claude Desktop can run MCP servers locally on your computer. Setting one up by hand means editing a JSON config file and installing its runtime.\n\n"
    "Desktop extensions package a local MCP server and everything it needs into a single bundle file (.mcpb) that installs with one click, like installing a browser extension.",

    "MCP servers come in two shapes. A local server runs on your own computer, usually started by the app and talking to it through standard input and output (stdio). A remote server runs on someone else's computer, often run by the service itself, and is reached over the internet.\n\n"
    "Remote servers usually need you to sign in (often OAuth); local ones can reach your files.",

    "Claude in Chrome is a browser extension that lets Claude act in your browser on your behalf: open pages, read them, click buttons and fill in forms, for example to compare prices or complete a routine web task.\n\n"
    "Because it acts as you, it works within permissions you control: which sites it may use and which actions need your approval.",

    "The principle of least privilege is a core security idea: every person, program or AI gets only the access it needs for its task, and no more.\n\n"
    "If something goes wrong (a bug, a prompt injection, a leaked token), the damage is limited to what that access allowed. Read-only access cannot delete anything.",

    "The Citadel taught that Research runs many searches on its own and writes a cited report. The Crossroads teaches that connectors give Claude access to your own sources, such as Google Drive, Gmail or a calendar.\n\n"
    "Combine the two: when connectors are enabled, Research is not limited to the public web.",

    "Connectors are how the Claude apps reach your accounts and services. You turn one on in settings, sign in to the service once through its own secure login, and Claude can then search and read from it in your chats.\n\n"
    "You never need to give Claude a password directly; the sign-in flow grants access without sharing it.",
},

// ================================================================ 5 Claude Code basics
{
    "Claude Code is Anthropic's coding agent that runs in your terminal (and in IDEs, the desktop app and the web). It reads your files, runs commands and makes edits directly in your project.\n\n"
    "It works on the folder you start it in, so the usual first step is to go to your project directory and start it there.",

    "Claude Code has permission modes that control how much it may do without asking: the default asks before edits and commands, auto-accept edits applies file changes without asking, and plan mode only researches and plans.\n\n"
    "There is a single keyboard shortcut that cycles through these modes while you type, and the current mode is shown below the input box.",

    "In plan mode Claude may read files and search the codebase, but it does not change anything. It explores, then presents a plan for you to approve or adjust.\n\n"
    "It is useful for bigger or riskier changes, when you want to agree on the approach before any file is touched.",

    "Claude Code works through a task in many small steps (read a file, run a command, edit). You do not have to wait for the whole task to finish to steer it.\n\n"
    "There is a key that stops Claude at the current step while keeping everything done so far, so you can type a correction and let it continue in the right direction.",

    "Claude Code saves checkpoints as it works, recording the state of the conversation and the files it edited. A checkpoint lets you return to an earlier moment if a change turns out badly.\n\n"
    "There is a command (and a keyboard shortcut) that opens the list of checkpoints so you can restore the code, the conversation or both.",

    "Claude Code can find files on its own, but naming the exact file saves time and avoids it reading the wrong thing.\n\n"
    "The prompt box has a mention syntax: type a special character followed by a path, and it autocompletes file names and adds that file to the conversation, much like mentioning a person in a chat app.",

    "Sometimes you want to run a quick command yourself, such as git status or a test run, without asking Claude to do it and without leaving the session.\n\n"
    "Claude Code has a prefix for this: the command runs directly in your shell, and its output lands in the conversation so Claude can see it too.",

    "CLAUDE.md is a file Claude Code reads automatically at the start of every session in that project. It holds things worth knowing every time: build and test commands, code style and project quirks.\n\n"
    "Rather than writing one from scratch, you can run a built-in command that scans your codebase and drafts a starter CLAUDE.md for you to edit.",

    "Everything in the session (your messages, file contents Claude read, command output) sits in Claude's context window. Leftover details from an old task take up space and can distract Claude on a new one.\n\n"
    "Claude Code has one command that summarizes the conversation and another that empties it entirely. Which fits when the old task has nothing to do with the new one?",

    "As a long session fills the context window, Claude has less room for new work. Claude Code can compress it: it replaces the detailed history with a summary of what matters, keeping the session going.\n\n"
    "You can add instructions saying what to keep (\"focus on the API changes\"). This also happens automatically when context gets nearly full.",

    "Claude Code saves your conversations locally, so closing the terminal does not lose them. You can reopen a past session with its full history and keep going.\n\n"
    "There are startup flags for this: one jumps straight back into your most recent session, and another shows a list of past sessions to choose from.",

    "Claude Code can write code quickly, but it cannot tell whether the code is right unless it can check. With no way to check, you become the only test.\n\n"
    "Giving Claude a feedback loop, such as a test suite, a build command, a linter or a screenshot to compare against, lets it run the check, see what failed and fix it before handing work back.",
},

// ================================================================ 6 Customizing Claude Code
{
    "CLAUDE.md files can live at several levels. One in a project folder applies to that project and is usually committed so the team shares it. One in your home directory's .claude folder applies to every project you open, and only you see it.\n\n"
    "The ~ symbol is shorthand for your home directory.",

    "Claude Code reads settings from settings.json files at several levels. .claude/settings.json in the project is meant to be committed to git so the team shares it. .claude/settings.local.json is for your personal overrides in that project and is git-ignored. ~/.claude/settings.json holds your personal settings for all projects.",

    "Permission rules in settings.json list tool uses that are allowed without asking or denied entirely. A rule names a tool and a pattern, such as Bash(git diff:*) for any git diff command, where :* means \"followed by anything\".\n\n"
    "Allowing a narrow, safe command saves you from approving it every time, while everything else still asks.",

    "Claude Code checks each tool call against your permission rules, which come in three lists: allow, ask and deny. A single call can match more than one rule, for example a broad allow and a narrow deny.\n\n"
    "Think about which way a security system should fail: if the rules disagree, the safer outcome is the one that does not run the action.",

    "A .env file usually holds secrets such as API keys and passwords, and it should never end up in Claude's context.\n\n"
    "A permissions deny rule blocks a tool use outright. Rules name the tool and a path, so a rule can block the tool that opens files from ever opening one specific file. A chat request is not enforced, and .gitignore only affects git.",

    "Custom slash commands are prompts saved as Markdown files. The file name becomes the command name, so review-pr.md becomes /review-pr.\n\n"
    "Commands in the project's .claude/commands folder are committed with the repo and every teammate gets them. Commands in ~/.claude/commands are personal and work in all your projects.",

    "A command file is a saved prompt, and you often want to pass something into it, such as an issue number or a file name: /fix-issue 123.\n\n"
    "Command files support a placeholder that Claude Code replaces with whatever you typed after the command name. It is written in capitals with a dollar sign in front.",

    "A Skill is a folder that teaches Claude how to do a specific kind of task: instructions, plus optional scripts, templates and reference files.\n\n"
    "Each skill has one required entry file whose frontmatter (a short header block at the top) gives the skill's name and a description of when to use it. Everything else in the folder is optional.",

    "Loading every skill's full instructions into every session would waste context on skills you are not using.\n\n"
    "Skills use progressive disclosure: Claude sees a short summary of each skill at first, and reads the full instructions (and any extra files) only when the task in front of it calls for that skill. That is why a good skill description matters.",

    "CLAUDE.md is loaded into context at the start of every session, so every line costs space every time. It should hold what Claude cannot easily work out but needs again and again.\n\n"
    "Good candidates: commands to build and test, style rules, and gotchas. Large dumps and secrets do not belong there, because it is often committed to git.",

    "Settings can come from several places: enterprise-managed policy (set by an organization's IT team), command-line flags, local project, shared project and user settings. When two disagree, a fixed order decides which wins.\n\n"
    "Think about who most needs the final say: an organization enforcing security rules across every developer.",

    "CLAUDE.md can import other files, so you can point to existing docs (a README, a style guide) instead of copying them in. The import syntax uses the same character you type in Claude Code's prompt box to mention a file, followed by the path.",
},

// ================================================================ 7 Hooks and automation
{
    "Hooks are your own shell commands that Claude Code runs automatically at specific moments, called events. A matcher narrows which tools a hook fires for, such as \"Edit|Write\" for file edits only.\n\n"
    "Some events fire before a tool runs, others after it finishes. Formatting a file only makes sense once the edit has been written.",

    "Some hook events fire before a tool runs, others after. A hook that runs before can inspect the tool call, such as the exact shell command, and stop it.\n\n"
    "To prevent a dangerous command, the check has to happen while the command has not run yet. The matcher names the tool that runs shell commands.",

    "Programs report how they ended with an exit code: 0 means success, other numbers mean different kinds of failure. Claude Code reads a hook's exit code to decide what to do next.\n\n"
    "One specific code is reserved for \"block this\": the action is stopped, and whatever the hook printed to stderr (the error output) is shown to Claude so it can adjust.",

    "When an event fires, Claude Code starts your hook script and passes it details about what is happening, such as the session, the tool name and the tool's input (the exact command or file path).\n\n"
    "These details arrive as structured JSON on the script's standard input, the same channel a command reads from when you pipe text into it.",

    "Instructions in CLAUDE.md are guidance: Claude reads and usually follows them, but it can forget or decide a step does not apply, especially in a long session.\n\n"
    "A hook is code that Claude Code runs every time its event fires, no matter what Claude decides. Use hooks for things that must happen every time, without exception.",

    "Claude Code fires hook events at key points in a session: when it starts, when you submit a prompt, around each tool call, when Claude stops and before compacting.\n\n"
    "Some events can add text to Claude's context, such as live data from a script. To load something once at the beginning of each session, pick the event named after that moment.",

    "There is a hook event that fires the moment you press Enter, before Claude reads your prompt. A hook on it can add context (a timestamp, the current ticket), check the prompt for problems, or block it, for example if it contains a secret.",

    "Normally Claude decides on its own when a task is finished and stops. Some hook events fire at that moment, and a hook can respond with a decision to block stopping, plus a reason (such as \"3 tests still fail\").\n\n"
    "That lets you build a check-and-continue loop: Claude keeps working until your condition is met.",

    "Headless or non-interactive mode runs Claude Code without the chat interface: you pass a prompt on the command line, it does the work, prints the result and exits.\n\n"
    "That makes it usable in shell scripts, cron jobs, git hooks and CI pipelines. It is a single short flag, short for print.",

    "When a script runs Claude Code, plain text output is hard for another program to use. Machine-readable means structured data, usually JSON, with clear fields for the result, cost, session id and so on.\n\n"
    "There is a flag that sets the output format, with a mode that returns one JSON object and a streaming mode that emits one JSON event per line as work happens.",

    "Claude Code can run inside GitHub's CI system, GitHub Actions. Once installed in a repository, anyone can mention @claude in an issue or pull request comment, and Claude reviews the code, answers questions or pushes fixes.\n\n"
    "There is a built-in command in Claude Code that walks you through installing the GitHub app and setting up the workflow.",

    "Hooks are shell commands that Claude Code runs automatically, with the same permissions as your user account and without asking each time. A hook can read, change or delete anything you can.\n\n"
    "That makes hooks from an untrusted source as risky as running an untrusted script. Read what a hook does before you add it.",
},

// ================================================================ 8 Subagents, MCP and plugins
{
    "A subagent is a separate Claude instance that the main session hands a task to. It has its own context window, its own instructions and optionally a limited set of tools.\n\n"
    "Searching a big codebase means reading many files, and most of them turn out to be irrelevant. Think about where all that reading ends up, and what the main conversation actually needs back.",

    "Subagents are defined as Markdown files with a frontmatter header (name, description, tools, model) followed by the agent's system prompt.\n\n"
    "Like commands, they live in a folder inside the project's .claude directory, so they can be committed for the team, or in ~/.claude for personal ones. The folder is named after what they are.",

    "A subagent's frontmatter contains fields such as name, description, tools and model. Claude reads one of these fields to decide whether a task should go to that subagent.\n\n"
    "Writing that field well (for example \"Use proactively after any code change to review for bugs\") is what makes delegation happen automatically.",

    "Each subagent can be given its own list of allowed tools. If you leave the list out, it inherits all the tools of the main session, including ones that edit files and run commands.\n\n"
    "Read, Grep and Glob only look at files and search them. Edit, Write and Bash can change things.",

    "A subagent's frontmatter can set which Claude model it runs on, independent of the main session. A smaller model is faster and cheaper, which suits simple, repetitive jobs like finding a file or summarizing a log.\n\n"
    "The model field takes a short alias for the tier.",

    "MCP servers can be added to Claude Code at three scopes. Local scope is for you, in this project only. User scope is for you, in every project. Project scope is written to a file in the repository root that you commit to git.\n\n"
    "Only the scope that lives in the repo travels to teammates when they pull.",

    "Claude Code manages MCP servers from the command line with the claude mcp subcommands (add, list, remove). A remote server is reached over HTTP at a URL, so adding one means telling Claude Code the transport type, a name for it and the address.",

    "Inside a Claude Code session, a short slash command opens a panel listing every configured MCP server and whether it is connected. For remote servers that use OAuth, this is also where you sign in.",

    "A plugin is a package that bundles Claude Code customizations so they can be installed together with one command, for example everything a team uses for its workflow.\n\n"
    "Plugins are shared through marketplaces, which are catalogs you add with /plugin. Think back over everything you have learned to customize: those are what get bundled.",

    "Git worktrees let one repository have several working folders, each checked out on its own branch, all sharing the same git history.\n\n"
    "Running a separate Claude Code session in each worktree means two features can be built at once without one session's edits overwriting the other's files.",

    "When Claude writes code, its context is full of its own reasoning and assumptions about why the code is right. Reviewing in that same context tends to repeat those assumptions.\n\n"
    "A writer/reviewer setup uses a second Claude with a clean context window that sees only the result, much like asking a colleague who did not write the code to review it.",

    "The Rapids taught the hook events: SessionStart, UserPromptSubmit, PreToolUse, PostToolUse, Stop and others. Stop fires when the main Claude finishes responding.\n\n"
    "Subagents get their own matching event that fires when a subagent finishes its task, named in the same style. You can use it to check or log the subagent's result.",
},

// ================================================================ 9 The Claude API
{
    "The Messages API is the HTTP endpoint behind every Claude product. Your code sends a JSON request and gets Claude's reply back as JSON.\n\n"
    "A request needs a few things every time: which Claude model to use, a cap on how long the reply may be (in tokens), and the conversation itself. Everything else, such as system, temperature and tools, is optional.",

    "In the Messages API, the conversation is a list of messages, each with a role of \"user\" or \"assistant\". Instructions that frame the whole conversation are not part of that list.\n\n"
    "Unlike some other APIs, Claude's request has a dedicated top-level field for them, separate from the messages array.",

    "Stateless means the server keeps no memory between requests. Each call is handled on its own, using only what that request contains.\n\n"
    "For a multi-turn chat, your code is responsible for the history: it keeps the earlier user and assistant messages and sends them along with each new message.",

    "Every response includes a stop_reason that says why Claude stopped generating. end_turn means it finished naturally. tool_use means it wants to call a tool. Another value means it hit the token limit you set in max_tokens, so the reply may be cut off mid-sentence.\n\n"
    "Checking stop_reason tells your code whether the answer is complete.",

    "Tool use (also called function calling) lets you describe your own functions to Claude. When Claude decides to call one, it stops and returns a structured block with the tool name and inputs, and your code actually runs the function.\n\n"
    "The stop_reason tells your code that this has happened and a tool call is waiting.",

    "In tool use, Claude asks for a tool call in a block with a unique id. Your code runs the tool and must hand the output back in the next request.\n\n"
    "The result goes in a content block of type tool_result, inside a message from the user's side. It references the id of the call it answers, so Claude can match results to requests when it made several calls at once.",

    "When you define a tool you give it a name, a description and a schema for its inputs: which parameters exist, their types and which are required.\n\n"
    "The schema format is a widely used standard for describing the shape of JSON data. Claude uses it to produce inputs that your code can trust.",

    "By default Claude decides whether to call a tool, and which one. The tool_choice parameter changes that: auto lets Claude choose, any requires some tool, tool requires one named tool, and none blocks tools.\n\n"
    "Forcing a specific tool is useful for structured extraction: define a tool whose schema is the output you want.",

    "Prompt caching lets the API reuse the processing of a prompt prefix that stays the same across requests, such as a long system prompt, documents or tool definitions.\n\n"
    "You mark where the stable part ends with a cache_control breakpoint. Later requests that start with the same prefix skip reprocessing it, which is cheaper and faster.",

    "With prompt caching there are three prices to know. Writing a prefix to the cache costs a bit more than normal input. Reading it back from the cache costs much less, a small fraction of the normal input price. Output is priced as usual.\n\n"
    "The read discount is why caching pays off whenever the same prefix is used many times.",

    "Some jobs do not need answers right away: overnight classification, evaluations, bulk summaries. The Message Batches API lets you submit many requests at once and collect the results later, usually within a day.\n\n"
    "In exchange for waiting, batch requests cost significantly less than the same calls made one at a time.",

    "HTTP 429 means \"too many requests\": you have hit a rate limit. A related status, 529, means the API is temporarily overloaded. Both are temporary.\n\n"
    "The standard response to temporary errors is exponential backoff: wait, retry, and double the wait after each failure, so many clients do not all retry at once. The official SDKs retry automatically.",
},

// ================================================================ 10 Building agents
{
    "The Claude Agent SDK is a library (for Python and TypeScript) for building your own agents. It gives you the agent harness that Claude Code is built on: the loop, built-in tools for files and commands, context management, permissions, subagents, hooks and MCP support.\n\n"
    "You bring the task and the prompts instead of writing all the plumbing yourself.",

    "An agent is a model working in a loop, using tools to make progress on a task, rather than answering once. Each turn of the loop has a few phases.\n\n"
    "Think about what a careful human does: find out what is going on, do something, check whether it worked, then decide what to do next.",

    "Anthropic describes several workflow patterns. Prompt chaining runs fixed steps in order. Routing sends each input to a specialized path. Parallelization runs pieces at once. Orchestrator-workers lets a lead model hand out subtasks.\n\n"
    "One more pattern pairs a producer with a judge in a loop, refining until the judge is satisfied. It works when there are clear criteria for good.",

    "Workflow patterns are predictable structures for combining model calls. Some run steps in a fixed order, some split work across parallel calls, and some send each input down a different path depending on what kind of input it is.\n\n"
    "A support desk that first sorts tickets into billing, technical and refund, then answers each kind with its own prompt, is a classic example.",

    "Anthropic separates workflows (code decides the steps) from agents (the model decides the steps). Agents are more flexible but slower, more expensive and harder to predict and debug.\n\n"
    "Every extra layer of autonomy is a cost you pay. Think about when that cost is worth it.",

    "A context window is the text a model can see at once. Bigger windows can hold more, but models do not attend to everything equally well.\n\n"
    "As more and more tokens pile up, details get harder to find and use, much like an overstuffed desk. This is why context engineering aims for the smallest set of high-signal tokens.",

    "There are two ways to give an agent information. You can load everything up front, filling context with data it may never need. Or you can give it ways to look things up, such as search, grep and file reads, and let it fetch what it needs when it needs it.\n\n"
    "Claude Code works the second way: it explores a codebase with search tools instead of reading every file.",

    "Tools are the agent's interface to the world, and Claude chooses among them by reading their names and descriptions. Overlapping or vague tools cause wrong picks, and huge outputs flood the context window.\n\n"
    "Good tool design is like good API design for a new teammate: clear names, clear purposes and results that return what matters.",

    "With the API there are two kinds of tools. Client tools are functions you define: Claude asks for a call, your code runs it and returns the result. Server tools, such as web search and code execution, are run by Anthropic while handling the request.\n\n"
    "The difference is where the tool actually executes.",

    "Computer use lets Claude control a computer through screenshots, mouse clicks and keyboard input. That is powerful, and risky: it can click the wrong thing, and web pages may contain prompt injection.\n\n"
    "Safe setups limit what can go wrong: an isolated environment, minimal access to accounts and data, logging, and a human sign-off before anything consequential.",

    "Evals (evaluations) are a set of test cases with clear success criteria, run automatically to measure how well your agent or prompt performs.\n\n"
    "They work like unit tests for AI behavior: when you change a prompt, model or tool, you rerun them and see whether things got better or worse, instead of judging by a few manual tries.",

    "Some agent tasks take hours and far exceed one context window. When the context fills, older details are summarized away (compaction), or a fresh session starts.\n\n"
    "Agents handle this the way people handle long projects: they keep written notes outside their head, such as a progress file, a to-do list or git commits, and read them back to pick up where they left off.",
},
};
