const languages = {
	"cpp": [
		[
			"and", "double", "not_eq", "throw", "and_eq", "dynamic_cast", "operator", "true", "asm", "else", "or", "try", "auto", "enum", "or_eq", "bitand", "explicit",
			"private", "typeid", "bitor", "extern", "protected", "typename", "bool", "false", "public", "union", "break", "float", "register", "unsigned", "case", "for", "reinterpret-cast",
			"using", "catch", "friend", "return", "virtual", "char", "goto", "short", "void", "if", "signed", "volatile", "compl", "inline", "sizeof", "wchar_t", "const", "int", "static",
			"while", "const-cast", "long", "static_cast", "xor", "continue", "mutable", "xor_eq", "default", "namespace", "switch", "delete", "new", "template", "do", "not", "this",
		],
		[
			"struct", "class", "typedef"
		],
		[
			"u32", "i32", "u64", "i64", "f32", "f64", "u8"
		]
	]
};

window.addEventListener("load", () => {
	[...Array.from(document.getElementsByTagName("code")), ...Array.from(document.getElementsByTagName("pre"))].forEach((code) => {
		if (!code.hasAttribute("lang")) return;
		const keywords = languages[code.getAttribute("lang")];
		if (keywords == null) return;

		const content = code.textContent.trim();
		const keywordMax = code.hasAttribute("keyword-max")? parseInt(code.getAttribute("keyword-max")):keywords.length;
		
		let updated = "";
		for (let i = 0; i < content.length;) {
			const slice = content.slice(i);
	
			let m = slice.match(/^(0x[0-9a-fA-F]+|([0-9]+(\.[0-9]*)?))/);
			if (m != null) {
				i += m[0].length;
				updated += "<span class='number'>" + m[0] + "</span>";
				continue;
			}
	
			m = slice.match(/^[a-zA-Z_][a-zA-Z_0-9]*/);
			if (m != null) {
				i += m[0].length;
				let type = "identifier";
				for (let i = 0; i < keywordMax; i++) {
					if (keywords[i].includes(m[0])) {
						type = "keyword-" + (i + 1);
						break;
					}
				}
				// const el = document.getElementById(m[0]);
				// if (el != null && el != code) updated += "<a class='" + type + "' href='#" + m[0] + "'>" + m[0] + "</a>";
				updated += "<span class='" + type + "'>" + m[0] + "</span>";
				continue;
			}
	
			m = slice.match(/^\/\/[^\n]*/);
			if (m != null) {
				i += m[0].length;
				updated += "<span class='comment'>" + m[0] + "</span>";
				continue;
			}

			if (slice.startsWith("/*")) {
				const end = slice.indexOf("*/");
				updated += "<span class='comment'>" + slice.slice(0, end + 2) + "</span>";
				i += end + 2;
				continue;
			}
	
			updated += content[i];
			i++;
		}
		code.innerHTML = updated;
	});
});