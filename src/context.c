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

    "Clarity is hard to judge from the inside. You know your project, so your own prompt always seems clear to you.\n\n"
    "A useful check borrows an outside reader's eyes: imagine handing the prompt to someone capable who knows nothing about your situation, and notice what they would have to ask before they could start. Those questions are the gaps.",

    "Constraints are the limits a good answer must fit: length (\"under 150 words\"), reading level (\"for a 12-year-old\"), things to include (\"mention the deadline\") and things to leave out (\"no pricing\"). Each one removes a guess.\n\n"
    "The question is about timing: when is the cheapest moment to tell Claude about a limit?",

    "Examples teach by pattern. If every example is a short, typical input, Claude learns that inputs are short and typical, and stumbles on the odd ones.\n\n"
    "Real inputs vary: long and short, clean and messy, easy and borderline. A good example set mirrors that variety so the only thing the examples share is the behavior you actually want.",

    "A prompt with examples mixes two kinds of text: instructions (what to do) and samples (what good looks like). Without clear boundaries, Claude may treat a line inside an example as an instruction, or blur where one example ends and the next starts.\n\n"
    "XML-style tags are a simple way to draw those boundaries: <examples> around the set, <example> around each one.",

    "XML tags in a prompt are labels such as <report> ... </report> that you invent to mark sections. Claude was trained on lots of structured text, so it reads them well.\n\n"
    "What makes them effective is not the specific words but how you use them: names that describe the content, the same name every time, and instructions that point to them (\"using the notes in <notes> tags\").",

    "Two good ideas can pull against each other. Reasoning first improves answers on hard tasks, but if your code needs only the result, a page of reasoning is in the way.\n\n"
    "Output tags solve both: ask Claude to put its working inside one named tag and the final result inside another. Your code then extracts just the part it needs.",

    "A prompt that runs on hundreds of inputs is closer to a program than a message, and like a program it has bugs you only find by running it.\n\n"
    "The usual loop: collect real inputs (including awkward ones), run the prompt, read the outputs that went wrong, change the prompt to address those failures, and run again.",

    "Context is the background Claude needs to do the job well: who it is for, what it is for, what you already have, and what finished looks like.\n\n"
    "For a cover letter, that means the job, your relevant experience and any limits such as length. Words like \"really good\" or \"professional\" add no information; facts do.",

    "A role is a short description of who Claude should act as, usually put in the system prompt: \"You are a senior SOC analyst reviewing alerts for a hospital.\"\n\n"
    "It doesn't give Claude new knowledge. It tells Claude which of its knowledge, vocabulary and judgment to bring to the front, and how to pitch its replies.",

    "Prompt chaining splits a big job into steps, each run as its own prompt. Each prompt is a separate request, so a later step only knows what you put in it.\n\n"
    "The glue between steps is you (or your code): take the output of one step and include it in the next prompt, usually wrapped in a descriptive tag such as <outline>.",

    "Complaints such as \"too long\" or \"less\" tell Claude something is wrong without saying what right looks like. It has to guess how much shorter.\n\n"
    "Concrete targets remove the guess: a number of sentences, a word limit, a format such as three bullets. The same principle as Promptwood's \"say what to do\" rule.",

    "With long documents, the relevant facts may be a few sentences buried in many pages. Answering straight away risks missing them or blending them with things that are not there.\n\n"
    "A two-step technique helps: first have Claude find and copy the passages that bear on the question (often into <quotes> tags), then answer using only those. It also lets you check the answer against the source.",

    "Negative instructions name a problem without naming the fix. Claude has to infer what a good result looks like, and the forbidden idea is still sitting in the prompt.\n\n"
    "Positive instructions describe the output you want: its style, length, audience or format. When you catch yourself writing \"don't\", try rewriting the line as \"do this instead\".",

    "When order matters (first check the data, then compute, then format), prose can blur which step comes first or hide a step in the middle of a sentence.\n\n"
    "Numbered lists are a simple structure Claude follows reliably. They also help you: when output is wrong, you can see which step failed.",

    "Hallucination means a confident answer that is not supported by the facts. It is more likely when a prompt implies an answer must always be given.\n\n"
    "Two habits help: limit Claude to the material you supplied, and tell it plainly that saying the answer is not there is a good response.",

    "When Claude summarizes or analyzes a document, you often need to trust specific claims. Without references, checking means re-reading the whole thing.\n\n"
    "Asking for supporting quotes (or citations) makes the answer auditable, and Claude can be told to retract any claim it can't find a quote for.",

    "Earlier models sometimes added flourishes on their own. Current models are tuned to follow instructions closely, so a plain request tends to get a plain result.\n\n"
    "If you want more, ask for more: describe the level of ambition, detail or polish you expect, and name extras such as animations or edge cases.",

    "Long-context prompts often include several documents. Without boundaries, Claude may confuse which fact came from which file.\n\n"
    "A consistent wrapper per document, with metadata such as the source or date, keeps them apart and lets you ask questions such as \"which document says this?\".",

    "Most real prompts run many times: one per email, ticket or document. The instructions stay the same; only the input changes.\n\n"
    "Separating the fixed part from the variables (often in double braces, wrapped in tags) makes the prompt easy to version, test and reuse, and works well with prompt caching.",

    "Sometimes you don't know what you forgot to mention. Claude often does: it can see where a request is ambiguous.\n\n"
    "Asking \"before you start, ask me any questions you need answered\" turns that into a short interview. It is the same idea as the golden test, run by Claude itself.",

    "Even strong reasoning can slip on one step. Asking Claude to verify its answer gives it a second pass with a different goal: finding problems rather than producing output.\n\n"
    "Good checks are concrete: recompute a total, test the code on an example, or compare the conclusion against the source.",

    "Self-correction chains are a form of prompt chaining. Step one writes a draft, step two critiques it against your standards, and step three fixes what the critique found.\n\n"
    "Because each step has one job, you can see and adjust each part. The criteria matter most: vague review requests produce vague edits.",

    "Roles work by setting Claude's perspective. A broad role helps a little; a precise one helps much more, because it tells Claude which knowledge and judgment to use.\n\n"
    "Add the domain, the kind of organization and the audience if they matter. The role and the task should agree.",

    "The same content needs different shapes in different places: a chat window, an email, a spreadsheet cell, a speech engine.\n\n"
    "Saying how the output will be used is context with a reason attached. Claude can infer the rules that follow from it, such as avoiding bullets or spelling out numbers.",

    "Prompt engineering is an experiment. If the goal is \"better\", every change looks like it might help.\n\n"
    "Specific, measurable criteria (correct category, under 100 words, cites the source, right tone) let you build test cases and compare prompt versions honestly.",

    "Plain \"think step by step\" lets Claude choose its own approach. That is often enough, but sometimes you know the right method.\n\n"
    "Then you can outline it: which factors to consider and in what order. The most structured form puts the reasoning in tags so your code can separate it from the answer.",

    "Older prompting advice used shouting to make sure an instruction was noticed. Recent Claude models are much more responsive to the system prompt.\n\n"
    "So heavy emphasis can overshoot. Prefer a normal sentence with a reason: \"Use the search tool when the question needs current information.\"",

    "Claude can help write prompts. It can point out unclear instructions, missing context and contradictions, or draft a structured version with tags and examples. Anthropic's Console also has prompt generation and improvement tools.\n\n"
    "Treat the suggestions as a draft: run your test cases on the new version before you switch.",

    "Few-shot prompting is pattern learning. Claude copies what the examples demonstrate: format, tone, length and approach.\n\n"
    "So if your examples include worked reasoning before each answer, Claude will reason the same way. It is a strong combination of examples and chain of thought.",

    "In a long prompt with several pieces of material, pronouns can point at more than one thing. Claude picks one, and it may not be yours.\n\n"
    "Be explicit: \"Compare the draft in <draft> with the style guide in <guide>, then fix the draft.\" Tags make this easy because each item has a name.",

    "In the API, the system prompt sits outside the message history and applies to every turn. It is a natural home for the role, standing rules and tone.\n\n"
    "Task-specific instructions and data can go in user messages. That split also makes it easy to reuse one system prompt with many different tasks.",

    "Describing a format in words leaves room for interpretation. Showing it does not.\n\n"
    "Include a short sample of the target output (fields, headings, order, length). For machine-read output, define the exact JSON shape or tag names and say that nothing else should be included.",

    "Claude picks up cues from how you write, not only what you write. Bulleted, heading-heavy prompts tend to get bulleted answers.\n\n"
    "If you want flowing prose, writing your prompt in prose helps, alongside a direct instruction. The same applies to tone: casual prompts invite casual answers.",

    "Single prompts work well for focused tasks. When a request bundles research, analysis, writing and formatting, Claude has to juggle them all at once.\n\n"
    "Chaining also lets you inspect intermediate results, fix the weak step and even run independent steps in parallel.",

    "Who reads the output shapes everything: which terms need explaining, how long sentences should be, what examples will land.\n\n"
    "Audience is one of the most valuable pieces of context, and it costs one sentence. Pair it with a format constraint if length matters too.",

    "When output is wrong, the question is usually \"what did Claude think I wanted?\" Its reasoning answers that.\n\n"
    "Ask it to reason in <thinking> tags or turn on extended thinking, then read where it went astray. Fix the prompt there rather than adding patches at the end.",
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

    "Claude comes in model tiers. Haiku is the smallest, fastest and cheapest. Sonnet balances intelligence and speed. Opus is the largest, built for the hardest problems.\n\n"
    "Bigger models are slower and use more of your limits, so the skill is matching the tier to the difficulty of the work.",

    "Bigger models cost more time and usage, and for many tasks they give the same result as a smaller one: reformatting a list, fixing a typo, answering a simple fact question.\n\n"
    "Think of it like choosing a tool from a toolbox. A sledgehammer is more powerful than a screwdriver, but not better at driving screws.",

    "The Claude apps (web, desktop and mobile) accept file uploads, and Claude can see as well as read.\n\n"
    "That means you can share the material you are working with directly, such as a contract PDF, a photo of a whiteboard, a screenshot of an error or a CSV export, instead of describing it or retyping it.",

    "Code execution gives Claude a sandbox: a private, temporary computer where it can write and actually run code.\n\n"
    "Running code means it can do real calculations on your data, produce charts, and save the results as files such as spreadsheets, documents and slide decks that you download.",

    "Normally each conversation starts fresh: Claude only knows what is in the current chat.\n\n"
    "The memory feature lets Claude carry useful context forward, such as your ongoing projects and preferences, and look back through past conversations when they are relevant, so you don't have to re-explain everything each time.",

    "A memory you can't inspect would be a black box: you wouldn't know what Claude assumes about you, or be able to fix something outdated.\n\n"
    "Claude's memory is designed to be transparent. Think about what control you would want over a colleague's notes about you.",

    "Styles change how Claude writes rather than what it knows: tone, length and formatting.\n\n"
    "The built-in presets cover common needs. One makes replies shorter and more direct, one adds teaching detail, and one makes the tone more formal. You can also make a custom style from your own writing.",

    "Each chat carries its entire history into every reply. That history helps when you are building on it, and gets in the way when you have moved on to something unrelated.\n\n"
    "Old details can steer new answers in odd directions, and re-reading a long history uses more of your limits.",

    "A Project is a workspace that groups related chats. Its instructions and knowledge are shared by the chats inside it.\n\n"
    "That is different from profile preferences, which follow you everywhere, and from a single message, which only affects one chat. Each level has a different reach.",

    "Web search and Research both bring in current information with citations, but they work at different scales.\n\n"
    "Search is a quick lookup inside a normal reply: a few searches, an answer in seconds. Research is a longer process that plans and runs many searches over several minutes and writes up a report.",

    "Vision means Claude can understand images, not just text. That includes photos, but also images full of information: charts, tables, diagrams, handwritten notes and screenshots of apps.\n\n"
    "For a chart, Claude can read the axes and values, describe trends and answer questions about the data.",

    "Extended thinking gives Claude a dedicated reasoning phase before it replies. It explores the problem, checks steps and considers alternatives.\n\n"
    "That extra work is not free. It takes more time, and the thinking is text that counts toward usage, just like the reply.",

    "Claude runs on the web, desktop apps for Mac and Windows, and mobile apps for iOS and Android. Your chats and projects sync across them.\n\n"
    "The mobile apps add things a phone is good at, such as voice conversations and snapping a photo for Claude to look at.",

    "Sometimes a conversation is worth showing: a good answer, a bug report, a worked example.\n\n"
    "A shared chat is a snapshot at the moment you shared it. Later messages are not added. Check for private details before you share, because anyone with the link can view it.",

    "Claude's answers vary from run to run. If one misses, a fresh attempt may be better.\n\n"
    "If several retries miss in the same way, the problem is probably the prompt. Then editing your message to add context works better than retrying.",

    "Claude's individual plans (Free, Pro, Max) are for one person. Organizations usually want more: seats, billing in one place, sharing and policy controls.\n\n"
    "Team and Enterprise add those. Enterprise adds further controls such as single sign-on and broader admin settings for large companies.",

    "Styles change how Claude writes without changing what you ask. Built-in options include Normal, Concise, Formal and Explanatory, and you can create your own.\n\n"
    "Pick a style per chat. Explanatory suits studying; Concise suits quick work; Formal suits drafts for official settings.",

    "A context window holds a limited amount of text. Small projects can be loaded whole; large ones can't.\n\n"
    "For large projects, Claude uses retrieval: it searches the project's files and brings the most relevant passages into the conversation. Clear file names and focused documents help it find the right material.",

    "Claude normally only knows what you type or upload. Connectors let it look things up in your own accounts.\n\n"
    "The Google ones are popular examples: find an email thread, check your week's meetings, or pull a document from Drive. You enable them in settings and can turn them off any time.",

    "Many important documents put key information in figures: financial charts, process diagrams, scanned tables.\n\n"
    "Claude processes PDF pages as text and as images, so it can answer questions about a chart or a diagram, not only the surrounding words.",

    "Long prompts read better with line breaks: a request, then the material, then your constraints.\n\n"
    "In the message box, Shift+Enter adds a line without sending. Pasted text keeps its line breaks automatically.",

    "A model learns from data collected up to a certain date. After that, it only knows what you tell it or what a tool finds.\n\n"
    "For anything that changes (news, prices, versions, schedules), turn on web search or paste in a current source, and check the citations.",

    "Search improves accuracy, but Claude can still misread or overstate a source, and some sources are wrong.\n\n"
    "Citations are there so you can follow the trail. A quick check of the original matters most for numbers, quotes and anything you will repeat to others.",

    "The three tiers trade capability against speed and cost. Haiku is fastest; Opus is most capable; Sonnet sits between.\n\n"
    "Many people use Sonnet by default, reach for Opus on the hardest problems and use Haiku for quick, simple or high-volume jobs.",

    "Very long conversations fill the context window with old turns, failed attempts and side topics. Quality can drop and usage goes up.\n\n"
    "Asking Claude to summarize decisions, facts and open questions, then pasting that into a new chat, keeps what matters and drops the rest.",

    "A subscription gives you a pool of usage. Chatting in the apps and running Claude Code both draw on it.\n\n"
    "If you use Claude Code heavily, a larger plan or API billing may suit you better. The same habits that save usage in chats, such as clearing context, help in Claude Code too.",

    "Feedback goes to Anthropic and helps improve the product. You can add a note explaining the problem.\n\n"
    "For getting a better answer right now, retrying or editing your message is the faster fix. Feedback is about the longer term.",

    "The context window is Claude's working memory for one conversation: your messages, its replies and every file you add.\n\n"
    "It is large enough for long reports or several documents at once. It is still finite, so very long chats or huge files can fill it, and Projects use retrieval when knowledge grows beyond it.",

    "With extended thinking on, Claude reasons before writing its answer. The app shows that reasoning in a collapsible section.\n\n"
    "Reading it is useful for checking the logic, seeing which assumptions it made and finding where a misunderstanding came from.",

    "Projects are workspaces: everything in them shares the same instructions and knowledge files.\n\n"
    "That is great for related work, but noise for unrelated questions. Starting the trip chat outside the coding Project keeps both clean.",

    "Code execution gives Claude a sandbox where it can run code on files you upload.\n\n"
    "For spreadsheets that means real calculations, not estimates: it reads every row, applies the logic and creates a downloadable file you can open in Excel or Sheets.",

    "Some context applies to almost every conversation: your role, experience level, tools you use, how you like answers.\n\n"
    "Profile preferences are a fixed note you write. Memory is built up from your chats and can also be viewed and edited. Either saves you repeating yourself.",

    "After a few weeks you may have hundreds of chats. Claude names chats automatically, and you can rename them and search your history.\n\n"
    "Grouping recurring work into named Projects keeps related chats together, along with their files and instructions.",

    "Claude can't see a page just because it exists. With web search or fetch enabled, it can open a URL. Otherwise, paste the text or attach a file.\n\n"
    "Pasting is the most reliable for pages behind logins or paywalls, which tools usually can't reach.",

    "Paid individual plans differ mostly in how much you can use. Pro suits regular use; Max raises the limits several times over.\n\n"
    "Heavy daily coding with Claude Code or very long analysis sessions are the typical reasons to move up. Check the plan page for current details.",

    "Images are a full input type. Claude can read photographed pages, screenshots, whiteboards and receipts.\n\n"
    "Clear, well-lit, straight-on photos work best. For long documents, a PDF scan keeps pages in order and is easier to work with.",
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

    "Artifacts come in several types, and each suits a different job. Documents hold text, Mermaid draws diagrams, SVG makes vector graphics, and HTML or React builds web pages and apps.\n\n"
    "Anything with buttons, inputs and calculations has to be code that runs.",

    "There are two main ways to store an image. Raster images (such as PNG and JPEG) are grids of pixels and get blurry when enlarged. Vector images describe shapes, lines and curves mathematically, so they can be drawn crisply at any size.\n\n"
    "Logos and icons are usually made as vectors for that reason.",

    "A document artifact holds formatted text (Markdown: headings, lists, tables) in its own panel.\n\n"
    "Like every artifact, it keeps a version for each change, so you can work on the report over several rounds, ask for edits to particular sections and go back to an earlier draft if needed.",

    "Some artifact types are just content to read, like a document or a diagram. Others are code that the panel actually executes.\n\n"
    "HTML pages and React components fall in the second group: the panel runs them like a small web page, so the interface is usable right away.",

    "Claude decides on its own whether a response deserves its own artifact panel. Usually that means content that is long, self-contained and likely to be reused, edited or shared.\n\n"
    "Its judgment won't always match what you want, and you can override it.",

    "Mermaid is a text-based language for diagrams, and it supports several diagram types: flowcharts, timelines, Gantt charts, sequence diagrams and more.\n\n"
    "A sequence diagram has a column for each participant (a user, an app, a server) and arrows between them, read top to bottom, showing who sends what and when.",

    "Most artifacts are fixed once Claude has written them: the text or the code doesn't change unless you ask for an edit.\n\n"
    "Some artifacts are small apps that send their own requests to Claude while you use them, for example to generate a question, give feedback or summarize text you paste in.",

    "Artifacts that call Claude can respond differently every time, to whatever the user types.\n\n"
    "That makes them a great fit for tools that need judgment or fresh content on each use. For content that never changes, a plain artifact is enough.",

    "Artifacts can be published, which gives them a link. Anyone with the link can open the artifact and use it, and people with a Claude account can remix it into their own copy.\n\n"
    "This is how a one-off answer becomes a small tool your team can use.",

    "When code fails, the error message usually names the problem and where it happened. \"It's broken\" gives Claude nothing to go on.\n\n"
    "The best bug reports, to people or to Claude, include the exact error text, what you did just before it appeared, and what you expected to happen instead.",

    "Code in a chat reply is fixed text in the message stream: to change it you get a new copy further down, and earlier copies scroll away.\n\n"
    "An artifact is a single living object in its own panel. Each update creates a new version you can compare or roll back to, and the latest one is always in the same place.",

    "Claude can read files you upload, including spreadsheets and CSVs.\n\n"
    "When building an artifact around real data, giving Claude the file lets it use the actual columns and values to shape the charts, tables and filters, rather than inventing sample data that you then have to replace.",

    "Claude weighs what you asked for. A full document, app, diagram or long piece of code that stands on its own and that you will likely change or keep is a good artifact.\n\n"
    "Conversational replies, brief snippets and answers that depend on the chat around them stay inline. You can always ask for an artifact explicitly.",

    "React artifacts run with a set of popular libraries available, including charting, icons and styling utilities such as Tailwind classes.\n\n"
    "Naming the library you want, or just asking for an interactive chart, helps Claude produce something that renders correctly in the artifact panel.",

    "Artifacts live in the conversation, but they are yours to take elsewhere.\n\n"
    "A downloaded HTML file opens in any browser; Markdown can go into your notes or docs; code can go into your project. Downloading also keeps a copy independent of the chat.",

    "A web page normally pulls in other files. Each extra file is something that can go missing when you share it.\n\n"
    "Asking for a single self-contained file is a simple constraint that makes the artifact portable, easy to download and easy to open in any browser.",

    "Mermaid describes diagrams in plain text, and Claude renders them as artifacts. It supports many types: flowcharts, sequence, state, class, entity-relationship, pie and Gantt charts.\n\n"
    "Pick by what you want to show: steps and decisions (flowchart), messages over time (sequence), states (state diagram), schedule (Gantt).",

    "Many systems have things that change status: orders, tickets, accounts. A state diagram makes the rules visible: which transitions are allowed and what triggers them.\n\n"
    "It is a good way to spot missing cases, such as how a locked account becomes active again.",

    "When an artifact's code fails to run, the panel can show the error and offer to have Claude fix it.\n\n"
    "It is a quick first step. If the same error keeps returning, describe what you were doing and what you expected, which gives Claude more to work with.",

    "Iterating on artifacts is like editing code: big batches of changes are hard to debug.\n\n"
    "One focused change per request, checked before the next, keeps the artifact working and the version history meaningful.",

    "Publishing makes an artifact viewable by anyone who has the link, and it can be remixed.\n\n"
    "Keep keys, passwords, private customer data and internal-only details out. AI-powered artifacts don't need your key anyway: viewers use their own Claude accounts.",

    "AI-powered artifacts call Claude from their code. Like the API, each call is independent.\n\n"
    "For a multi-turn experience, the artifact stores the messages in its own state and includes them in each request. Ask Claude to build it that way when you describe the app.",

    "Inside an AI-powered artifact, Claude's reply is input for code, not just text for a person. The same rule as in Promptwood applies.\n\n"
    "Ask for structured output, show the exact shape and handle parse errors gracefully. That makes the app robust.",

    "Claude can analyze uploaded data with code and produce charts or files, which is great for one-off answers.\n\n"
    "When you want an interactive view you will refine and share (filters, charts, summaries), an artifact is the better home: it runs in the panel and keeps every version.",

    "Artifacts run real code in your browser, so interactive things work: keyboard input, animation, scores.\n\n"
    "Describe the rules, controls and look you want, then iterate one feature at a time. Games are also a fun way to practise refining artifacts.",

    "Remixing is copy-and-modify. The new owner starts from your artifact and can change anything without touching yours.\n\n"
    "That makes sharing low-risk: people can adapt your tool for their needs while you keep yours unchanged.",

    "Accessible design includes readable contrast, labels for screen readers, keyboard navigation and layouts that work on small screens.\n\n"
    "It is easier to include from the start than to add later. One sentence in your first request covers it.",

    "People often open shared links on their phones. A layout designed for a wide screen can overflow or squash on a narrow one.\n\n"
    "Ask Claude to make the layout responsive and to test the narrow case: stacked columns, wrapping text and touch-sized buttons.",

    "Artifacts are easier to design when there is something to display. Placeholder data in the same format (same fields and types) shows how the real thing will look.\n\n"
    "Tell Claude the shape of the real data so the switch is a simple replacement.",

    "Many small edits can leave leftover code, inconsistent naming and fragile structure.\n\n"
    "When changes start breaking unrelated things, a fresh rewrite with a clear list of current features often helps. Version history keeps the old version if anything is lost.",

    "Raster images are grids of pixels and blur when enlarged. SVG describes shapes, so it stays sharp at any size.\n\n"
    "Because SVG is text, Claude can create and edit it precisely: change a color, move an element, simplify a shape.",

    "Not every code artifact is meant to run in the panel. Python scripts, SQL queries and config files are code you take elsewhere.\n\n"
    "A code artifact keeps the code separate from the discussion, with versions as you refine it, and a copy button for moving it into your project.",

    "AI-powered artifacts combine code and a prompt. Both can fail: the code on unexpected replies, the prompt on unusual inputs.\n\n"
    "Try empty input, very long input, off-topic requests and inputs that try to change the artifact's instructions, then ask Claude to fix what breaks.",

    "Artifacts are part of the conversation, so they appear wherever you open it.\n\n"
    "That is another reason to ask for responsive layouts: a calculator or flashcard app you built on your laptop may get its most use on your phone.",

    "A name is a compact statement of what the artifact is for. It helps Claude choose features and labels, and helps viewers understand it at a glance.\n\n"
    "Pair the name with audience and key features, as you learned in Promptwood, for the best first version.",

    "Version history lets you look back, but you may want pieces of several versions.\n\n"
    "Be specific about what to keep: \"use the two-column layout from the earlier version and keep the new revenue chart.\" Claude then builds a new version with both.",
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

    "MCP (Model Context Protocol) is a standard way to connect AI applications to tools and data.\n\n"
    "Before standards like USB, every device needed its own special cable and port. A single standard means any device with the plug works with any computer with the socket. MCP aims to do the same for AI integrations.",

    "MCP has two sides. A server exposes capabilities, such as the tools to work with GitHub or a database. A client is the application a person actually uses; it connects to servers and makes their capabilities available to the model.\n\n"
    "One client can connect to many servers.",

    "An MCP server can offer three kinds of things. Tools are actions the model can call. Resources are data the model can read.\n\n"
    "The third kind, prompts, are pre-written templates that the server provides for the user to pick, often shown as a menu or slash command, such as a standard code-review request with the right instructions built in.",

    "Remote MCP servers run on the internet, usually operated by the service itself. Many of them are listed in one place inside the Claude apps, where you can find them, add them and sign in.\n\n"
    "That is easier than setting up a server yourself.",

    "A local MCP server is a program running on your own computer. It can do things a server on the internet can't, such as reading and writing files in your folders or controlling apps installed on your machine.\n\n"
    "In Claude Desktop, desktop extensions make installing these local servers easy.",

    "Claude in Chrome lets Claude act in your browser as you, using your logged-in sessions.\n\n"
    "That is powerful, and risky if it acts on the wrong site. The extension uses permissions you control: which sites it may work on, and which actions need your approval first.",

    "Beyond the Claude apps themselves, Claude is available inside other tools people already work in, such as team chat and spreadsheets.\n\n"
    "There it can work with the content in front of you, such as a channel's discussion or a sheet's data, without you copying everything over.",

    "Prompt injection is text planted in content an AI reads (a web page, an email, a document) that tries to give it new instructions.\n\n"
    "The defense is to keep two things separate: instructions come from the user, and everything else is data to read and analyze. Suspicious instructions inside data are a warning sign, not a command.",

    "Even good tools can make mistakes: a misunderstood request, a prompt injection, a bug.\n\n"
    "The impact of a mistake depends on the action. Reading or searching is easy to recover from. Sending money, deleting data or emailing people outside your organization can't be undone, so those deserve a human check.",

    "A connector links Claude to an outside service, such as storage, email, calendars, code hosting or team chat, usually through an MCP server.\n\n"
    "Once connected, Claude can search and read your content there, and sometimes take actions, all within the permissions you granted when you signed in.",

    "A language model learns from a large set of training data collected up to a cutoff date. After training, it has no live connection to the world.\n\n"
    "Everything else it knows in a conversation comes from what is put into that conversation: your messages, uploads, and the results of any tools it is allowed to use.",

    "OAuth is the standard behind \"Sign in with...\" buttons. Instead of giving an app your password, you sign in on the service's own page and approve specific access, such as \"read your calendar\".\n\n"
    "The service then gives the app a token limited to that access, which you can revoke at any time.",

    "MCP has three roles. The host is the AI application the user works in. Inside it, a client manages the connection to a particular server. The server provides tools, resources or prompts.\n\n"
    "One host can connect to many servers at once, each through its own client.",

    "A transport is how messages travel between client and server. With stdio, the client launches the server as a local process and talks through standard input and output.\n\n"
    "Streamable HTTP is for servers reached over the network. It replaced an earlier HTTP with Server-Sent Events transport.",

    "JSON-RPC is a simple standard for calling methods with JSON. A request names a method and parameters; a response carries a result or an error.\n\n"
    "Building on an existing standard is part of why MCP SDKs exist in many languages and why servers and clients interoperate.",

    "Besides what servers offer (tools, resources, prompts), MCP defines features that clients offer to servers.\n\n"
    "Sampling is one: the server requests an LLM completion through the client, so the server needs no model or API key of its own. The client can show the request to the user for approval.",

    "Sometimes a server can't continue without a decision only the user can make. Elicitation lets it ask, with a defined format for the answer.\n\n"
    "The client shows the request, and the user can respond, decline or cancel. Servers should never use it to ask for passwords or other secrets.",

    "A filesystem server could in principle touch any file the process can reach. Roots let the client say \"these are the folders that matter for this task\".\n\n"
    "They guide the server's scope. Real enforcement still comes from permissions and the operating system.",

    "Resources are data a server exposes: files, database rows, API records. A URI gives each one a stable address.\n\n"
    "Servers can also offer resource templates with parameters, such as a URI pattern for any user's profile.",

    "Debugging through a full AI app is slow: you have to prompt the model just to trigger a tool.\n\n"
    "The Inspector connects directly to your server so you can list capabilities, call tools with chosen inputs and see raw responses and errors.",

    "The connectors directory lists reviewed connectors, but any remote MCP server can be added as a custom connector.\n\n"
    "Only add servers you trust: a connector can see what Claude sends it and return content that ends up in your conversation.",

    "Local servers run on your machine with your permissions. Remote servers receive the data Claude sends to their tools.\n\n"
    "Either kind can return content designed to inject instructions. Check who publishes a server, what it accesses and whether it is maintained.",

    "Each capability is fine alone. Together they create an exfiltration path: untrusted content plants instructions, private data is read, and an outbound tool sends it away.\n\n"
    "Breaking any leg helps: limit private access, avoid untrusted inputs, or require approval for anything that sends data out.",

    "Claude reads tool descriptions to decide how to use tools. That makes them a channel for injection.\n\n"
    "A poisoned description might tell Claude to send extra data to the tool. Vetting servers and reviewing what they expose reduces the risk.",

    "A tool's name and schema say what it takes, not how risky it is. Annotations add that: read-only, destructive, idempotent, or reaching the open world.\n\n"
    "They are hints from the server, so a client should not rely on them from an untrusted server.",

    "Remote servers often act on your behalf in another service. MCP's authorization spec builds on OAuth 2.1.\n\n"
    "You sign in on the service's own page and approve specific access. The token can be limited and revoked, which a shared password can't.",

    "Connectors often bundle read tools (search, get) and write tools (send, delete) together.\n\n"
    "Allowing reads automatically while keeping approval for writes balances convenience with control. Review these settings when you add a connector.",

    "In a company, connectors touch company data, so the organization needs a say.\n\n"
    "Admins can choose which connectors are available. Members then connect their own accounts, getting only the access their own credentials allow.",

    "Everything a tool returns becomes part of the conversation. Huge results crowd out other context and can make answers worse.\n\n"
    "Ask Claude to filter (date range, owner, status) or to request a summary instead of raw data. Good servers also paginate and truncate by default.",

    "stdio is the simplest transport: no network, no ports. The host launches the server, often with a command such as npx or uvx, and messages flow over stdin and stdout.\n\n"
    "Because it runs locally, it can reach your files and apps. That is the power and the risk.",

    "Claude can run as an add-in in tools you already use. In Excel, it can read the workbook, trace how cells relate and make changes.\n\n"
    "As with any tool that edits your files, review what it changed, especially in models others depend on.",

    "Claude's Slack integration lets teams work with Claude where they already talk: summarizing threads, drafting replies or answering questions.\n\n"
    "Workspace admins control installation. Mind what is in a thread before asking Claude to act on it, as with any shared content.",

    "Least privilege means granting the minimum access a task needs. It limits damage from bugs, mistakes and malicious content.\n\n"
    "A review-only token can still read code and comment if you allow it, but it can't push, merge or delete. Grant more only when a task truly requires it.",

    "Tools can fail silently, act on the wrong item, or partly succeed. Claude reports what the tool returned, which may not be the full story.\n\n"
    "For consequential actions (sending, paying, publishing), a quick check of the real system is good practice.",

    "The SDKs handle the protocol details: message formats, transports and capability negotiation.\n\n"
    "You write the useful parts, such as what a tool does. That is why many services ship their own MCP server alongside their API.",

    "MCP connections start with a handshake. The client and server agree on a protocol version and declare features, such as whether the server offers tools or resources.\n\n"
    "After that, the client can list tools, read resources and call tools based on what was negotiated.",
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

    "Claude Code started as a terminal tool, but it is available in several places: editor extensions, the Claude desktop app and the browser.\n\n"
    "They share the same core agent and your settings, so the choice comes down to where you like to work.",

    "Claude Code's permission modes control how often it stops to ask you. In the default mode it asks before editing files and running commands.\n\n"
    "Auto-accept edits is a middle ground for when you trust the direction of the work and don't want to approve every file change.",

    "Claude Code normally asks before risky actions. There is a way to turn off every permission check, so Claude runs any command without asking.\n\n"
    "That can be useful for fully automated work, but a wrong command could delete files or leak data. Think about where that risk is acceptable.",

    "The context window is everything Claude can see in the current session: the system prompt, CLAUDE.md, your messages, every file it has read and every command output.\n\n"
    "It has a fixed size. /context gives a breakdown of what is taking up that space right now.",

    "Jumping straight into code often leads to fixing the wrong thing or fighting the existing design.\n\n"
    "A reliable rhythm has four steps: read the relevant code first, agree an approach (plan mode helps), make the change, then save it to version control with a clear message.",

    "Claude Code is not limited to text. You can paste or drag images into the prompt.\n\n"
    "That is handy for error dialogs, UI bugs, design mockups to match and diagrams. An image often carries details you would forget to type.",

    "Claude Code works best with the same things any good prompt has: what is wrong, where it happens and how you will know it is fixed.\n\n"
    "For bugs, a reproduction (such as a failing test) is especially valuable. It proves the bug exists and later proves the fix works.",

    "Claude Code asks before running commands it doesn't have permission for. When it asks, you can approve that single use, or approve that command so it doesn't ask again.\n\n"
    "Routine, safe commands like running tests are good candidates for the second option.",

    "Claude Code takes checkpoints as it works: snapshots of the conversation and of the files Claude has edited.\n\n"
    "If a change goes wrong you can return to a checkpoint, and you get a choice about what to restore. Sometimes you want the old code back, sometimes the old conversation, sometimes both.",

    "Claude Code can run git: it can check status, view diffs, create branches, write commits and open pull requests.\n\n"
    "A finished, tested change should be saved in version control so it is reviewable, shareable and easy to undo later.",

    "A model's context is like a desk: the more papers piled on it, the harder it is to find the right one.\n\n"
    "In a long session, old file contents, finished discussions and long command outputs all stay in context. The details that matter for the current task have to compete with all of it.",

    "Claude Code can search and read through a project on its own: folder structure, key files, how pieces connect.\n\n"
    "Asking it questions like \"how does authentication work here?\" or \"give me an overview of this repo\" is a low-risk way to start, since it only reads.",

    "Claude Code's default permissions are tiered by risk. Read-only actions such as reading files and searching code run freely so Claude can explore.\n\n"
    "Actions that change things, such as editing files or running commands, ask first. You can widen or narrow this with modes and permission rules.",

    "Permission prompts are also a steering moment. A bare \"no\" leaves Claude guessing why.\n\n"
    "Saying what to do instead (\"use the existing helper\" or \"don't touch the migration\") turns a refusal into useful guidance.",

    "Claude Code has many built-in slash commands, plus custom commands from your project, your user folder and plugins.\n\n"
    "/help is the quick way to see what's available. Typing / also shows a filtered list as you type.",

    "Different tasks suit different models. You might use a more capable model for planning a tricky change and a faster one for routine edits.\n\n"
    "/model changes the model during a session; settings can set your default.",

    "Longer prompts are clearer with line breaks. Pressing Enter normally sends.\n\n"
    "A trailing backslash works in any terminal. /terminal-setup can configure Shift+Enter in terminals that need it. Pasting multi-line text also works.",

    "Checkpoints are a safety net for code edits. They don't capture what happens outside the files Claude edited.\n\n"
    "For risky commands, rely on other safeguards: backups, staging databases, git commits, and approving such commands yourself.",

    "Checkpoints are tied to the session. Git commits last, can be compared with diff, and work with every other tool you use.\n\n"
    "Committing before a large change means you can always inspect exactly what changed or reset if the result isn't what you wanted.",

    "Bigger tasks have many parts. Claude Code tracks them as a list, marking items done as it goes.\n\n"
    "You can read the list to check the plan matches what you want, and redirect early if a step is wrong or missing.",

    "Code often looks odd for a reason that's recorded in history: a bug fix, a workaround, a requirement.\n\n"
    "Asking \"look through git history to explain why this was done\" lets Claude read commits and blame, which is great for onboarding and safer refactors.",

    "Claude Code works with the tools in your environment. With git and gh installed and authenticated, it can create branches, write commit messages and open pull requests.\n\n"
    "You stay in control through permissions: you approve pushes and PR creation unless you allow them.",

    "You don't have to wait for Claude to finish. Messages you send during work are picked up and factored in.\n\n"
    "For a real change of direction, Esc to interrupt is better. For an added detail, just send it.",

    "Plan mode separates thinking from doing. Claude explores and proposes; nothing changes.\n\n"
    "When you accept, it switches to a mode that can edit, and you can choose whether to approve edits one by one. You can also keep refining the plan before approving.",

    "Both free up context. /clear throws the conversation away, which is right when the old discussion is irrelevant.\n\n"
    "/compact replaces the conversation with a summary, so key decisions survive. You can add instructions such as \"focus on the API changes\".",

    "Context is finite. When it nears the limit, Claude Code summarizes the conversation so far automatically.\n\n"
    "Auto-compaction keeps you working, but summaries lose detail. Clearing between tasks and compacting yourself at natural break points gives you more control.",

    "If something seems wrong (updates failing, commands missing, odd errors), /doctor checks the setup and reports problems.\n\n"
    "Running it is a good first step before searching for help.",

    "On first run, Claude Code asks you to log in. Subscriptions share usage with the Claude apps; API billing charges per token.\n\n"
    "Organizations can also run it through cloud providers. /login and /logout switch accounts.",

    "Approving edits one by one is slower but gives you a chance to read each change.\n\n"
    "Many people start in the default mode on unfamiliar code, then switch to auto-accept once they trust the direction.",

    "TDD fits Claude Code well. Tests define what done means, and Claude can run them repeatedly.\n\n"
    "Ask Claude to write the tests, check they fail for the right reason, commit them, then implement without changing the tests.",

    "Library docs and changelogs often hold the answer to \"how do I use this API?\".\n\n"
    "Giving Claude the URL is more reliable than relying on its training. Permission rules can limit which domains it may fetch.",

    "Running in the cloud means work continues without your laptop and you can start several tasks at once.\n\n"
    "The results come back as branches or pull requests you review. It is a good fit for well-defined tasks you want to hand off.",

    "Some commands never finish by design: dev servers, file watchers, log tails.\n\n"
    "Claude Code can run these in the background and read their output when needed, for example starting a server, then testing against it.",

    "Like a shell, Claude Code keeps a history of what you typed.\n\n"
    "Recalling a prompt is handy when you want to run a similar request again with a small change.",

    "Large changes fail in large ways. Breaking work into stages, each with tests passing, limits how far a mistake can spread.\n\n"
    "Ask Claude to plan the stages first, then work through them, committing after each passes.",

    "Claude Code changes quickly, with new commands and features arriving often.\n\n"
    "Automatic updates keep you current. If you need the newest version right away, run claude update. The changelog shows what's new.",
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

    "Claude Code can read several CLAUDE.md files. One in your home folder holds your personal rules, and one at the project root holds the team's.\n\n"
    "Large projects can also have CLAUDE.md files in subfolders, such as frontend/ or api/, with rules for just that part of the code. Think about when those rules would be relevant.",

    "CLAUDE.md files are Claude Code's long-term memory: instructions loaded into every session.\n\n"
    "You can edit them in any text editor, but Claude Code also has a slash command, named after the feature, that opens them for you.",

    "Claude Code reads settings from several JSON files. Some are shared with the team through git, and some are just for you.\n\n"
    "The personal-but-project-specific one has \"local\" in its name and is automatically ignored by git, so your experiments never land in the team's settings.",

    "When settings files disagree, Claude Code uses a precedence order to decide which wins.\n\n"
    "The general rule is that a more specific scope overrides a broader one. Think about which file applies more narrowly: one shared by the whole team, or one that is just yours for this project.",

    "Permission rules live in settings files, but you don't have to edit JSON by hand.\n\n"
    "Claude Code has a slash command, named after what it manages, that lists your current allow, ask and deny rules and lets you add or remove them.",

    "A permission rule has two parts: a tool name, and optionally a pattern in brackets. Bash(npm run test:*) only matches test commands, and Read(./.env) only matches that one file.\n\n"
    "Think about what a rule matches when the brackets are left off entirely.",

    "Permission rules go into three lists. One lets matching actions run without asking, one blocks them completely, and one always pauses for your confirmation.\n\n"
    "The third suits actions that are sometimes fine and sometimes not, such as git push.",

    "Custom slash commands are Markdown files whose names become commands.\n\n"
    "Where you put the file decides who gets it: commands in a project's .claude/commands folder are shared with the team through git, while commands in the .claude folder in your home directory follow you to every project.",

    "Slash command files are saved prompts. To make them reusable, they can include a placeholder that is filled in when you run the command.\n\n"
    "Claude Code swaps the placeholder for exactly the text you typed after the command's name before sending the prompt.",

    "A Skill is a folder that teaches Claude a specific task. The SKILL.md file holds the core instructions.\n\n"
    "Often a task also needs supporting material: a script that does a fiddly conversion reliably, a template to fill in, or a reference document too long to include every time.",

    "Skills are a general format for packaging instructions and resources, not a feature of one product.\n\n"
    "The same SKILL.md folder can be used in the Claude apps, added to Claude Code, and used through the API, so one skill can serve you everywhere.",

    "CLAUDE.md is loaded automatically at the start of every Claude Code session.\n\n"
    "Context space is limited and shared with everything else in the session, so a long CLAUDE.md takes up space every time, even when most of it has nothing to do with the task at hand.",

    "In a monorepo you might start Claude in packages/api. The repo root's CLAUDE.md still matters there.\n\n"
    "Claude Code reads memory files from the current directory upward, so shared rules at the root combine with package-specific ones. Subfolders below load when Claude works in them.",

    "Some tools need environment variables: a flag that disables telemetry, a test database URL, a path.\n\n"
    "Putting them under env in settings applies them automatically. Don't commit secrets in shared settings; use personal settings or a secret manager.",

    "Teams working on sensitive code sometimes want every session to start read-only.\n\n"
    "Setting defaultMode does that. You can still switch modes during a session unless managed policy prevents it.",

    "By default, Claude Code works inside the folder you started it in. Some work spans folders, such as a frontend and a separate API repo.\n\n"
    "--add-dir on the command line or additionalDirectories in settings grants access to extra paths.",

    "Web access is useful but is also a path for data to leave or for untrusted content to arrive.\n\n"
    "Domain rules let you allow trusted documentation sites while keeping approval (or a deny) for everything else.",

    "A rule such as Bash(curl:*) matches commands starting with curl. The same request could be made with another tool, a variable, or a script.\n\n"
    "For real protection, layer defences: limit network access, use a PreToolUse hook that inspects commands, or run in a sandbox.",

    "MCP tools are named mcp__<server>__<tool> in Claude Code. Permission rules use the same names.\n\n"
    "Allowing a whole server is convenient for read-only servers. For servers with write tools, allow specific tools instead.",

    "Custom commands can carry their own settings in YAML frontmatter, including description and argument-hint.\n\n"
    "allowed-tools is handy for commands such as /commit that need git commands: they run smoothly without approving each one.",

    "$ARGUMENTS inserts everything after the command as one string. Positional arguments split it into words.\n\n"
    "They are useful when a command needs separate pieces, such as an ID and a priority, in different places.",

    "Commands often need current information, such as the diff or the branch name.\n\n"
    "Embedding a shell command gathers it automatically each time. The command needs to be allowed, often through allowed-tools in the frontmatter.",

    "A typed command is like a button you press. A skill is expertise Claude reaches for when it recognizes a matching task.\n\n"
    "That is why a skill's description matters so much: it is what Claude reads to decide whether the skill applies.",

    "Like commands and subagents, skills have project and personal locations.\n\n"
    "Project skills are committed with the repo so the whole team gets them. Plugins can also bundle skills.",

    "Loading everything up front would waste context. A skill can be large without cost until it's used.\n\n"
    "Names and descriptions load first, SKILL.md loads when the skill is triggered, and linked files load only when Claude needs that detail.",

    "Claude chooses skills from their descriptions alone. Vague descriptions cause skills to be missed or misused.\n\n"
    "Include concrete capabilities and trigger situations. Keep it brief: it loads into context in every session.",

    "Skills work across Claude products. In the apps you upload a ZIP of the skill folder; in Claude Code you place it in a skills directory; in the API you upload it through the skills endpoints.\n\n"
    "Code execution must be enabled for skills that run scripts.",

    "Settings come from several places. Managed policy wins; then command-line arguments; then local project, shared project and user settings.\n\n"
    "Flags are handy for a one-off change without editing files.",

    "Repeated mistakes usually mean Claude lacks a project fact: a command, a convention, a trap.\n\n"
    "Adding that fact to CLAUDE.md makes the fix permanent. Keep notes short and specific, and remove ones that no longer apply.",

    "Instructions that don't change Claude's behaviour waste context; instructions that are ignored need rewording.\n\n"
    "Check whether rules are followed, emphasize the important ones and delete outdated or obvious ones.",

    "You can edit settings.json by hand, but many settings are easier to change interactively.\n\n"
    "/config shows the options and saves your choices to the right file.",

    "Project memory belongs with the code. Committing CLAUDE.md means it's versioned, reviewed and updated alongside the project.\n\n"
    "Personal preferences belong in your user-level memory instead, so they don't affect teammates.",

    "Organizations can deploy managed policy settings to enforce rules, such as disabling bypass mode or denying certain tools.\n\n"
    "Because they sit at the top of the precedence order, users and projects can't override them.",

    "Blocking network tools is one way to reduce the risk of data leaving your machine.\n\n"
    "As with any Bash pattern, determined workarounds exist, so combine deny rules with other controls for sensitive work.",

    "Read and Edit rules accept glob patterns, similar to gitignore patterns.\n\n"
    "Scoped patterns let you auto-approve edits to safe areas while keeping approval for config, migrations or infrastructure files.",

    "Settings follow the same rule as memory: shared things go in the project, personal things in your user folder.\n\n"
    "Display preferences affect only you, so they shouldn't be committed to the repo.",
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

    "Hooks are configured alongside Claude Code's other settings, in the same JSON settings files that hold permission rules.\n\n"
    "Because those files exist at user, project and local levels, a hook can be personal, shared with the team through git, or local to one project.",

    "A matcher filters which tools a hook fires for. It is matched against the tool's name, and it supports simple patterns.\n\n"
    "In patterns like this, the vertical bar | is the common symbol for \"or\", the same as in many programming and search tools.",

    "Programs report how they ended with an exit code. Claude Code reads a hook's code to decide what to do.\n\n"
    "0 means all is well. One specific code (2) is reserved for deliberately blocking an action. Any other non-zero code is treated as the hook itself having a problem, which you should know about but which shouldn't stop Claude's work.",

    "Claude Code sometimes needs your attention, for example to approve a command, or because it has been waiting for input.\n\n"
    "There is a hook event for exactly these moments, so you can route the alert anywhere: a desktop notification, a sound, a phone push or a chat message.",

    "When a long session fills up its context, Claude Code compacts it: it replaces the detailed history with a summary.\n\n"
    "The detail is gone after that. One hook event fires just before this happens, giving a script a chance to save or process the full transcript first.",

    "Claude Code's hook events come in pairs and stages: before and after a tool, when a prompt is submitted, when Claude stops, and at the start and end of a session.\n\n"
    "Cleanup work, such as writing a final log entry or removing temporary files, belongs at the very end.",

    "The simplest hooks communicate only through their exit code: 0 to continue, 2 to block.\n\n"
    "For richer behavior, a hook can print structured JSON to standard output. Claude Code reads that JSON for decisions such as allowing, denying or asking about a tool call, or for extra context to add.",

    "Deny rules in settings block tool uses that match a pattern, such as Bash(rm:*). They are simple and fast, but they can only match patterns.\n\n"
    "Some decisions need real logic: \"block git push only on Fridays\" or \"block any command that touches the production database\". That needs a program.",

    "Headless mode (claude -p) runs Claude Code once from the command line without the interactive interface.\n\n"
    "Like many command-line tools it reads standard input, so the Unix pipe | can send another command's output (a log, a diff, a file) straight into the prompt.",

    "When a script runs Claude Code, it usually wants structured results it can parse. JSON is the standard format for that.\n\n"
    "There are two JSON flavors. One waits and returns a single object when the run is finished. The other streams a line of JSON for each event as it happens, useful for showing progress or reacting in real time.",

    "Hooks on tool events receive the full details of each tool call as JSON on standard input, including the exact command for the Bash tool.\n\n"
    "A hook script can do anything with that information, including writing it to a file. Matching on the Bash tool limits the hook to shell commands.",

    "Hooks can be set in personal settings or in the project's shared settings file, which is committed to git.\n\n"
    "Personal hooks only protect your sessions. If a rule matters for the whole codebase, such as auto-formatting or blocking writes to a protected folder, consider where it should live.",

    "Exit codes give a simple pass or block. JSON output gives finer control.\n\n"
    "A PreToolUse hook can approve a call without a prompt, deny it with a reason Claude sees, or force the usual permission prompt. That lets you encode real policy in code.",

    "For most events, stdout is shown to you in the transcript rather than sent to Claude.\n\n"
    "UserPromptSubmit and SessionStart are the exceptions: printing text adds it as context. That makes them easy places to inject the date, the branch, or open tickets.",

    "Prompt-level hooks can act as a filter, for example blocking prompts that contain secrets.\n\n"
    "Exit code 2 blocks the prompt and shows your stderr message to the user, so they know why.",

    "Stop hooks that block stopping are powerful: \"tests still fail, keep going\". Without care, they can block forever.\n\n"
    "The hook input includes stop_hook_active. A hook should check it, and its own conditions, before blocking again.",

    "Hooks run in the middle of Claude's work. A slow hook slows every matching action.\n\n"
    "Keep hooks fast. For slow jobs, a longer timeout is possible, but consider running the slow part elsewhere.",

    "Hooks may run while Claude's working directory is a subfolder. Relative paths to scripts can then break.\n\n"
    "Referencing scripts as \"$CLAUDE_PROJECT_DIR\"/.claude/hooks/check.sh keeps them working wherever Claude is.",

    "A typical entry: under \"hooks\", the event \"PostToolUse\" has an array of matcher groups. Each has a \"matcher\" such as \"Edit|Write\" and a \"hooks\" array.\n\n"
    "Each hook has a type, usually \"command\", with the command to run and an optional timeout.",

    "Command hooks are scripts: deterministic and fast. Some decisions need judgment: is this task really done?\n\n"
    "Prompt-based hooks use a model to evaluate and return a decision. They're slower and less predictable, so use them where a script can't decide.",

    "Hooks can target MCP tools the same way as built-in tools.\n\n"
    "A regex such as mcp__memory__.* catches every tool from that server, so you can log or guard them as a group.",

    "Matchers narrow a hook to particular tools. Leaving them open applies the hook everywhere.\n\n"
    "That suits logging and auditing; for formatters and guards, a specific matcher avoids unnecessary runs.",

    "Sessions start in several ways: new, resumed, after /clear, or after compaction.\n\n"
    "Matching lets you load context only when it makes sense, such as re-adding key notes after compaction.",

    "Hooks can come from user settings, project settings, local settings, managed policy and plugins.\n\n"
    "/hooks gives one place to see what's active, which helps when an unexpected hook is blocking something.",

    "Because hooks run commands automatically, a silent change to settings could be dangerous.\n\n"
    "Claude Code captures hooks when a session starts. If the files change, it warns you and the new hooks apply after review.",

    "Headless runs have no one to click approve. Anything not allowed is denied.\n\n"
    "List exactly what the job needs, such as Edit and Bash(npm test:*), rather than skipping all permission checks.",

    "In automation, a run that loops for a long time wastes money and blocks the pipeline.\n\n"
    "A turn limit is a simple guard. Choose a number that fits the task, and treat hitting it as a signal to investigate.",

    "Multi-step automations sometimes need Claude to remember earlier steps.\n\n"
    "Capture the session_id from JSON output, then pass it to --resume in the next call to continue with full context.",

    "Workflow files are visible to anyone who can read the repo.\n\n"
    "GitHub secrets are encrypted and injected at run time. The setup for Claude Code GitHub Actions stores the key this way.",

    "Running in the cloud doesn't change how Claude Code works. It still loads project memory and settings from the repo.\n\n"
    "That's one more reason to keep CLAUDE.md accurate: it guides local sessions and automated reviews alike.",

    "Large migrations and analyses are repetitive. One giant session fills its context and mixes files together.\n\n"
    "A shell loop running claude -p per file gives each a clean context. Add --allowedTools and an output format your script can read.",

    "Automated jobs often need a little extra direction: a reviewer persona, an output rule, a focus area.\n\n"
    "Appending keeps Claude Code's built-in behaviour and adds yours on top. For instructions that apply to every run in a repo, CLAUDE.md is usually the better home.",

    "Some files should only change through tools: lockfiles, generated code, migrations.\n\n"
    "A hook reads tool_input.file_path, compares it to a protected list, and exits 2 with a message so Claude knows to use the proper command.",

    "Every hook gets JSON on stdin: session ID, transcript path, working directory and event name, plus event-specific fields.\n\n"
    "For tool events that includes tool_name and tool_input. Scripts often use jq to pull out tool_input.command or tool_input.file_path.",

    "Blocking a single tool call lets Claude try another approach. Sometimes you want everything to stop.\n\n"
    "Setting continue to false stops processing entirely, for example when a hook detects that something has gone seriously wrong.",

    "Timing matters. PreToolUse can stop an action; PostToolUse can only respond to it.\n\n"
    "PostToolUse hooks are ideal for formatting, linting and checks that report problems back to Claude for the next step.",
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

    "Claude Code ships with some built-in subagents, so you get delegation without writing any yourself.\n\n"
    "One of them specializes in finding and reading code in a large repository without filling the main conversation with every file it opens. Its name describes the job.",

    "Subagents are defined as Markdown files in a folder named agents inside a .claude directory.\n\n"
    "Like commands and settings, there is a project location (inside the repo, shared with the team) and a personal location (inside your home folder, available in every project).",

    "Subagents are Markdown files, but you don't have to create them by hand.\n\n"
    "Claude Code has a slash command, named after what it manages, that lists your subagents and helps you create and edit them, including choosing their tools.",

    "A subagent file has two parts. The frontmatter at the top (between --- lines) holds settings such as name, description, tools and model.\n\n"
    "The rest of the file is plain text: the instructions that define how the subagent thinks and works, given to it as its system prompt every time it runs.",

    "Claude Code delegates to subagents automatically when a task matches a subagent's description.\n\n"
    "Automatic delegation won't always pick what you had in mind, and sometimes you know exactly which specialist you want. You can always override it with a plain request.",

    "Claude Code manages MCP servers with claude mcp subcommands. A local server is a program Claude Code starts on your machine and talks to through stdio.\n\n"
    "So adding one means giving it a name and the exact command that launches the server, separated by -- so the server's own options aren't mistaken for Claude Code's.",

    "MCP servers in Claude Code can be added at three scopes: just you in this project, just you in all projects, or shared through a file in the repo.\n\n"
    "One of these is used when you don't specify a scope. Sharing with the team is an explicit choice, not the default.",

    "Plugins package commands, subagents, skills, hooks and MCP servers so they install together.\n\n"
    "Claude Code has a slash command for working with them, named after the thing it manages, where you can browse what's available, install, enable and remove them.",

    "Plugins have to come from somewhere. Claude Code uses catalogs called marketplaces that list available plugins.\n\n"
    "A marketplace can be public or private. A team can host its own, for example in a git repository, so everyone installs the same internal tools.",

    "Some tasks depend on each other (you can't test code before writing it). Others are independent, such as researching three unrelated modules.\n\n"
    "Independent tasks don't need to wait in line. Claude can hand each one to its own subagent, and they work at the same time, each in a clean context.",

    "Normally a git repository has one working folder, with one branch checked out at a time. Switching branches swaps the files under you.\n\n"
    "Worktrees let a repository have several working folders at once, each on a different branch, all sharing the same history and objects. Each one is an independent place to work.",

    "Each subagent can be given a tools list in its frontmatter, such as Read, Grep, Glob, to limit what it may do.\n\n"
    "The field is optional. Think about the most convenient default when the field is left out entirely, and why that default makes it important to restrict reviewers and other read-only helpers.",

    "Delegation goes one level deep: the main conversation delegates to subagents, and they do the work and report back.\n\n"
    "That keeps behaviour predictable and costs bounded. Plan delegation from the main session.",

    "A subagent can name its own model alias, or inherit from the main session.\n\n"
    "Inherit keeps behaviour consistent; naming a model lets you trade capability for speed and cost per job.",

    "Like skills, subagents are chosen from their descriptions.\n\n"
    "A description that names trigger situations, and phrases such as \"use proactively\", makes delegation more likely without you asking each time.",

    "A subagent may read dozens of files and run many commands. None of that floods the main context.\n\n"
    "The main conversation gets the result. Ask subagents to report what matters: findings, file paths and recommendations.",

    "Each subagent starts with a clean context, so it has to re-discover what it needs.\n\n"
    "That's worth it for big searches and parallel work. For small tasks that need context you already have, doing them directly is cheaper.",

    "The project version reflects the team's needs for that codebase, so it wins.\n\n"
    "This mirrors how project settings refine personal ones: the more specific scope takes precedence.",

    "Claude Code ships with built-in subagents for common jobs, such as Explore for searching and general-purpose for complex multi-step tasks.\n\n"
    "In plan mode, research is delegated so the main context stays focused on the plan.",

    "Claude Code has three MCP scopes. local: just you, this project (the default). project: shared with the team via .mcp.json. user: just you, every project.\n\n"
    "Personal tools like a notes server suit user scope; team tools like the project database suit project scope.",

    "Pulling a repo shouldn't silently start software that can access your system.\n\n"
    "Claude Code asks before using servers from .mcp.json. Check what a server is before approving.",

    "Shared configuration should describe how to connect, not include credentials.\n\n"
    "Variable expansion fills values from each developer's environment at run time, with defaults where needed.",

    "Different servers can expose tools with the same name, such as search. Namespacing avoids collisions.\n\n"
    "The same names appear in permission rules, hook matchers and subagent tool lists.",

    "Prompts are templates a server offers. Claude Code lists them among slash commands.\n\n"
    "Typing / shows them, and arguments can be passed after the command name.",

    "Typing @ suggests files and also resources from connected servers.\n\n"
    "Mentioning a resource fetches it into context, such as an issue, a document or a database schema.",

    "The claude mcp command manages servers from the shell: add, list, get, remove.\n\n"
    "Inside a session, /mcp shows the status of each server and handles signing in to servers that need authentication.",

    "The manifest names the plugin and holds metadata such as version and author.\n\n"
    "Its commands, agents, skills, hooks and MCP servers sit in standard folders beside it.",

    "A marketplace is a catalog of plugins, often a git repository with a manifest.\n\n"
    "After adding one, browse and install plugins from it with /plugin. Companies can run internal marketplaces.",

    "Plugins package commands, skills, subagents, hooks and MCP servers. Hooks run shell commands automatically; MCP servers run as processes.\n\n"
    "Review what a plugin contains and who maintains it before installing.",

    "Consistency across a team matters for tooling. Settings in .claude/settings.json are shared through git.\n\n"
    "When teammates trust the folder, Claude Code offers to install the listed plugins.",

    "Worktrees share one repository but have separate working folders. Each can be on a different branch.\n\n"
    "Start Claude in each folder for parallel sessions. Remove worktrees with git worktree remove when done.",

    "Files ignored by git, such as node_modules, virtual environments or .env files, aren't shared between worktrees.\n\n"
    "Run your setup in each worktree. A script that prepares a fresh worktree saves time.",

    "Multi-Claude workflows use separation as a quality tool. A test writer that hasn't seen the code tests the requirements.\n\n"
    "The implementer then has an independent target. Fresh-context reviewers apply the same idea.",

    "Test runs, builds and log searches produce lots of text. Most of it doesn't help.\n\n"
    "A subagent can run the command, read the output and return a short summary, keeping the main session's context for the actual work.",

    "Delegation has overhead: explaining the task and the subagent re-gathering context.\n\n"
    "For quick edits that build on what you just discussed, the main session is faster and has the context.",

    "Many people configure MCP servers in Claude Desktop first. Setting them up again by hand is tedious and error-prone.\n\n"
    "The import command reads Desktop's configuration and lets you choose which servers to add. Check the scope you import them into.",
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

    "Every request to the Claude API must prove who is calling, using a secret API key from the Claude Console.\n\n"
    "The key travels in an HTTP header, a name-value pair sent with the request. Its name has the common x- prefix for custom headers. Never put the key in client-side code.",

    "APIs evolve, and a change that is an improvement for some users can break others' code.\n\n"
    "The Claude API asks each request to state which version of the API it was written for, in a header named after the company. Your code keeps working the same way until you choose to move to a newer version.",

    "Every response says why generation stopped, in the stop_reason field.\n\n"
    "You can give the API a list of stop sequences, strings such as \"</answer>\" where Claude should stop writing. When one of them appears, generation halts, and the stop_reason names the cause.",

    "A message's content is not limited to a single string. It can be a list of content blocks, each with a type: text, image, document (for PDFs), tool_use and tool_result.\n\n"
    "Images and PDFs go into the conversation as their own blocks, alongside your text, inside a user message.",

    "Without streaming, your code waits until Claude has written the entire reply, then gets it all at once. Long replies can take a while.\n\n"
    "With streaming, the API sends small pieces as they are generated, using server-sent events (SSE), a standard way for a server to push a series of messages over one HTTP connection.",

    "A streamed response arrives as a series of typed events. Some mark the structure: the message starting, a content block starting or stopping, the message ending.\n\n"
    "Others carry the actual content in small increments. In programming, \"delta\" often means \"the change since last time\".",

    "Extended thinking gives Claude a reasoning phase before its final answer. In the apps it is a toggle; in the API it is a request parameter.\n\n"
    "You set it along with a token budget for the reasoning, and the response then includes the thinking as separate blocks before the answer.",

    "Prompt caching saves the processed form of a prompt prefix so later requests can reuse it.\n\n"
    "Caches are not kept forever. Each entry has a time to live; using the entry resets that clock, and there is a paid option for a longer lifetime. Infrequent requests may find their cache has already expired.",

    "Tokens are the units the API counts and bills: input tokens for everything you send, output tokens for what Claude writes. Each model also has a maximum context size.\n\n"
    "For big or variable prompts, it helps to know the token count before you send, to estimate cost or keep under limits.",

    "The messages array holds the conversation, one turn at a time. Each message has a role that says who said it: the person (or your application) or Claude.\n\n"
    "Standing instructions are not part of this back-and-forth. They go in a separate top-level system field.",

    "Tool use lets Claude work with functions you define. Claude never runs your code directly: it describes the call it wants, and your application executes it.\n\n"
    "Agents are built by repeating that exchange: Claude asks for a tool call, your code runs it and sends back the result, and Claude decides what to do next.",

    "Anthropic publishes official client libraries (SDKs) for languages such as Python and TypeScript. They wrap the HTTP API in normal functions and objects.\n\n"
    "Anything they do you could write yourself: setting headers, building requests, parsing responses, retrying on rate limits. The question is why you would want to.",

    "A response can contain several blocks: text, tool_use, thinking. That's why content is an array rather than one string.\n\n"
    "Simple code often reads content[0].text, but robust code loops over blocks and checks their type.",

    "Billing is per token, so usage is your meter. Logging it per request shows where costs come from.\n\n"
    "With caching, usage also shows tokens written to and read from the cache, which is how you confirm caching works.",

    "Temperature controls how much sampling varies. Higher values give more varied, creative output.\n\n"
    "Even at 0, output isn't guaranteed identical across calls. Some newer model features restrict sampling parameters, so check the docs for your model.",

    "stop_reason tells your code why generation ended, which decides what to do next.\n\n"
    "end_turn: show or use the answer. max_tokens: it was cut off. tool_use: run the tool. stop_sequence: one of your stop strings appeared.",

    "The messages array alternates user and assistant turns, beginning with user.\n\n"
    "There is no \"system\" role inside messages. Tool results also travel in user turns, as tool_result blocks.",

    "Aliases are convenient for experiments. In production, a silent model change can shift outputs and break assumptions.\n\n"
    "Pin a version, then upgrade deliberately after running your evals on the new one.",

    "Claude decides whether and how to call a tool from its definition alone.\n\n"
    "The description matters most: explain what the tool does, when to use it, what each parameter means and any limits. Detailed descriptions improve tool use.",

    "tool_choice options: auto (the default; Claude decides), any (must use some tool), tool (must use the named one) and none.\n\n"
    "Forcing tools is useful for structured extraction, where a tool's schema defines the output you want.",

    "Claude can request several tools at once when they are independent, which saves round trips.\n\n"
    "Run them (in parallel if you like) and send all results back in one message. Missing results cause an error.",

    "Failures are normal: timeouts, bad input, missing records. Hiding them makes Claude reason from false information.\n\n"
    "Mark the result as an error and say what went wrong and how to fix it. Claude often retries correctly.",

    "Asking for JSON in a prompt usually works, but not always. Structured outputs constrain generation so the response matches the schema.\n\n"
    "It also applies to tool inputs with strict mode. Check the docs for supported models and schema features.",

    "Embedding a big file in every request repeats the upload.\n\n"
    "With the Files API you upload once and pass a file_id in document or image blocks. Combine with prompt caching to also save on processing.",

    "When you pass documents and enable citations, Claude's answer comes with structured references to the supporting text.\n\n"
    "Your app can show them so users can verify claims, which is valuable in legal, research and support tools.",

    "Extended thinking produces reasoning before the answer. That reasoning is generation, and it counts toward output.\n\n"
    "Use thinking where it improves results, and control its size with the thinking settings your model supports.",

    "When Claude thinks, then calls a tool, the thinking is part of its turn. Your next request includes that assistant turn.\n\n"
    "The blocks carry a signature, so they must be returned exactly. The SDKs make this easy if you append the response content as-is.",

    "Caching has overhead, so very short prefixes aren't cached. The minimum varies by model.\n\n"
    "Caching shines with large stable prefixes: long system prompts, documents, tool definitions and long conversations.",

    "The prompt is assembled as tools, system, then messages. A cache hit needs the prefix to match exactly.\n\n"
    "Keep the stable parts early and unchanged, and put variable content at the end.",

    "The first request pays extra to store the prefix; later ones read it cheaply.\n\n"
    "Caching pays off when a prefix is reused several times within its lifetime. A longer-lived cache option costs more to write.",

    "The default cache is refreshed when used, so steady traffic keeps it warm. Sparse traffic lets it expire.\n\n"
    "Choose the lifetime that matches your request pattern, and check cache read tokens in usage to confirm.",

    "Batches trade immediacy for cost. You submit many requests, then poll or check back for results.\n\n"
    "They suit work with no user waiting: evaluations, bulk classification, content generation.",

    "Rate limits cover requests and tokens per minute. Response headers report your limits and remaining capacity.\n\n"
    "Back off when limited. Spread traffic, use batches for bulk work, and cache to reduce input tokens.",

    "Status codes say whose problem it is. 4xx errors usually need you to change the request: 400 invalid, 401 auth, 429 rate limit.\n\n"
    "529 and 500 come from the service side and are usually temporary.",

    "API keys carry billing and access. Exposing one lets others use your account.\n\n"
    "Keep keys server-side, rotate them if leaked, and use separate keys per environment.",

    "A stream runs: message_start, then for each block content_block_start, deltas and content_block_stop, then message_delta and message_stop.\n\n"
    "Text arrives in content_block_delta; stop_reason and final usage arrive near the end in message_delta.",
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

    "Anthropic describes several workflow patterns for combining model calls. One runs pieces of work at the same time, and comes in two flavors.\n\n"
    "Sectioning splits a task into independent parts handled at once, such as checking code for security issues and style issues separately. Voting runs the same task several times and compares the answers.",

    "Some tasks can't be split into fixed steps in advance. A code change might touch two files or twenty, depending on what you find.\n\n"
    "One workflow pattern handles this with a lead model that looks at the task, decides what subtasks are needed, sends each to a worker call, and then brings the results together.",

    "Workflows follow paths you define in code. Agents choose their own steps in a loop, using tools and feedback.\n\n"
    "Agents are more flexible but slower, more expensive and less predictable. That cost is only worth paying when a predefined path can't handle the problem.",

    "Agents make mistakes like anyone else. The difference between a fragile agent and a reliable one is whether it notices.\n\n"
    "The agent loop ends each cycle by checking the work. The check can be clear rules (does it match the format?), automated tests, or another model judging the result.",

    "Agents working on long tasks keep accumulating context: tool results, files, messages. Eventually the context window fills up.\n\n"
    "One technique is to summarize the history so far, keeping the important decisions and state, then continue with that summary in place of the full detail.",

    "When an agent calls a tool incorrectly (a bad parameter, a missing file), the tool's response is the agent's only clue about what went wrong.\n\n"
    "Good tool design treats error messages as instructions for the next attempt, just as good error messages help human developers.",

    "With the API, there are two kinds of tools. Client tools are functions you define and run in your own code. Server tools are built in and run by Anthropic while it handles the request.\n\n"
    "Built-in server tools include web search, web fetch and code execution.",

    "Computer use lets Claude operate a computer by looking at screenshots and sending mouse and keyboard actions.\n\n"
    "It reads whatever is on screen to decide what to do next, and screens show content from many sources: web pages, emails, documents and pop-ups, some of which could be hostile.",

    "An eval needs a grader: something that decides whether each output passed. There are two main kinds.\n\n"
    "Code-based graders check things like exact matches, valid JSON or required keywords. They are fast, cheap and consistent. Model-based graders use an LLM to judge qualities that are hard to measure in code, such as helpfulness or tone.",

    "Evals are a set of test cases with success criteria, used to measure an agent or prompt.\n\n"
    "An eval can only measure what it covers. A set made only of easy cases gives a high score while real users hit the hard ones: unusual inputs, missing data, ambiguous requests.",

    "A deployed agent acts in the real world, often without a person watching each step. That calls for the same safety practices used for any powerful automation.\n\n"
    "Three ideas cover most of it: limit what it can access, keep a record of what it does, and require human approval for actions that are hard to undo.",

    "Context engineering is the practice of keeping an agent's context small and high-signal.\n\n"
    "A subagent has its own separate context window. When a task needs lots of reading and searching, a subagent can do that work in its own context and send back only what matters, keeping the main agent's context clean.",

    "Anthropic's guide on building agents starts here. Every workflow and agent is built from models given capabilities.\n\n"
    "Getting this block right (clear tools, good retrieval, a sensible memory) matters more than elaborate orchestration.",

    "Chains break work into steps. Errors in early steps flow into later ones.\n\n"
    "A gate, such as checking an outline has required sections, catches problems early, when they're cheap to fix.",

    "The Agent SDK packages the harness behind Claude Code: tool execution, context management, permissions and more.\n\n"
    "It is available for Python and TypeScript, which covers most agent backends and scripts.",

    "Built-in tools cover files, shell and web. Real agents need domain tools: query orders, check inventory.\n\n"
    "The SDK lets you write those as functions and register them, with no separate server process required.",

    "The same patterns from Hookfall Rapids apply. Hooks can inspect tool calls before they run, and permissions limit what's allowed.\n\n"
    "Code-based checks don't depend on the model remembering a rule.",

    "Prompts fail in two ways. Too low: brittle logic that breaks on unexpected cases. Too high: vague advice that gives no direction.\n\n"
    "Aim for the middle: clear principles and examples, organized in sections.",

    "Agents reason in language. Cryptic IDs carry no meaning, and mixing them up is easy.\n\n"
    "Return names, titles and short descriptions. If later calls need an ID, include it alongside a readable label.",

    "As agents gain tools, similar names cause wrong choices.\n\n"
    "Grouping by service or resource in the name, plus distinct descriptions, keeps choices clear.",

    "Instead of hoping the model remembers a rule, change the interface. Require absolute paths, validate inputs, use enums for fixed choices.\n\n"
    "This idea came from Anthropic's work on agents for coding benchmarks.",

    "Tools are a prompt for the agent. When an agent misuses one, the transcript shows why.\n\n"
    "Claude can read many transcripts, spot patterns and propose clearer descriptions or interfaces. Re-run evals to confirm.",

    "An agent might solve a task in one run and fail in the next.\n\n"
    "Running cases several times measures how often it succeeds. For production, consistent success matters more than occasional success.",

    "There are often several right ways to complete a task.\n\n"
    "Focus graders on the end state: the file is fixed, the ticket is resolved. Check process only where it matters, such as forbidden actions.",

    "A score says how often; a transcript says why.\n\n"
    "Reading them also catches eval bugs: tasks that are impossible, or graders that reward the wrong thing.",

    "Agents working over days can't keep everything in context.\n\n"
    "The memory tool lets Claude save notes, progress and learned facts, then consult them later. Your application controls where the files live.",

    "Tool results pile up: file reads, searches, command output. Many are useless a few steps later.\n\n"
    "Context editing clears old tool results based on rules, which complements compaction and memory.",

    "Parallel subagents explore different directions, each in its own context, and the lead combines results.\n\n"
    "That works well for broad research. The cost is significant, so it suits high-value tasks.",

    "When subtasks constantly depend on each other's details, agents spend effort coordinating and can conflict.\n\n"
    "Much of coding is like this, which is why single agents with subagents for side tasks are common there.",

    "Each new context window starts without memory of the last. The agent relies on what it wrote down.\n\n"
    "Anthropic's harness research suggests a feature list, a progress file and git commits, with work done incrementally.",

    "Agents decide their own next step, and sometimes they get stuck retrying.\n\n"
    "A turn or time limit, plus a clear success condition, keeps them bounded. Hitting the limit should be logged and reviewed.",

    "Tests are a proxy for correctness. An agent under pressure to make them pass may take shortcuts.\n\n"
    "Clear instructions, blocking edits to test files with hooks or permissions, and reviewing diffs keep the target honest.",

    "Agents take many steps. When something goes wrong, you need to know which step and why.\n\n"
    "Log prompts, tool calls, results and decisions. Traces also feed evals and help explain behaviour.",

    "The same skill format from Config Keep works for agents.\n\n"
    "A skill for, say, building spreadsheets loads only when relevant and can include scripts the agent runs, keeping the base prompt lean.",

    "Context is precious. Moving bulk data through it is slow, costly and can degrade attention.\n\n"
    "With code execution, an agent can load data in a sandbox, compute what it needs and bring back a compact result.",

    "Requiring approval for everything makes agents useless; requiring it for nothing is risky.\n\n"
    "Focus approval on what is hard to undo or affects others: money, deletions, external messages, production systems.",
},
};
