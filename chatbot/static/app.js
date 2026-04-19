const chatBox = document.getElementById("chat-box");
const input = document.getElementById("input");
const sendBtn = document.getElementById("send-btn");
const endBtn = document.getElementById("end-btn");
const knowledgeCount = document.getElementById("knowledge-count");

let history = [];
let isStreaming = false;

// 초기 지식 개수 표시
fetchKnowledgeCount();

function addMessage(role, text) {
  const div = document.createElement("div");
  div.className = `message ${role}`;
  div.textContent = text;
  chatBox.appendChild(div);
  chatBox.scrollTop = chatBox.scrollHeight;
  return div;
}

function setLoading(loading) {
  isStreaming = loading;
  sendBtn.disabled = loading;
  endBtn.disabled = loading;
  input.disabled = loading;
}

async function sendMessage() {
  const message = input.value.trim();
  if (!message || isStreaming) return;

  input.value = "";
  addMessage("user", message);

  setLoading(true);
  const botDiv = addMessage("bot", "");

  try {
    const res = await fetch("/chat", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ message, history }),
    });

    if (!res.ok) throw new Error("서버 오류");

    const reader = res.body.getReader();
    const decoder = new TextDecoder();
    let fullReply = "";

    while (true) {
      const { done, value } = await reader.read();
      if (done) break;
      const token = decoder.decode(value, { stream: true });
      fullReply += token;
      botDiv.textContent = fullReply;
      chatBox.scrollTop = chatBox.scrollHeight;
    }

    // 히스토리 업데이트
    history.push({ role: "user", content: message });
    history.push({ role: "assistant", content: fullReply });

    // 최근 20턴(10쌍)만 유지
    if (history.length > 20) {
      history = history.slice(history.length - 20);
    }
  } catch (err) {
    botDiv.textContent = "오류가 발생했습니다: " + err.message;
  } finally {
    setLoading(false);
    input.focus();
  }
}

async function endSession() {
  if (history.length === 0) {
    addMessage("system", "저장할 대화 내용이 없습니다.");
    return;
  }

  setLoading(true);
  addMessage("system", "대화 내용을 분석하여 지식을 저장 중입니다...");

  try {
    const res = await fetch("/extract", {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({ history }),
    });
    const data = await res.json();

    if (data.added > 0) {
      addMessage("system", `✓ ${data.added}개의 새로운 사실이 저장되었습니다.\n${data.extracted.join("\n")}`);
    } else {
      addMessage("system", "저장할 새로운 사실이 없습니다.");
    }

    await fetchKnowledgeCount();
    history = [];
  } catch (err) {
    addMessage("system", "지식 저장 중 오류가 발생했습니다.");
  } finally {
    setLoading(false);
    input.focus();
  }
}

async function fetchKnowledgeCount() {
  try {
    const res = await fetch("/knowledge");
    const data = await res.json();
    knowledgeCount.textContent = `저장된 지식: ${data.facts.length}개`;
  } catch {
    // 조용히 무시
  }
}

// 이벤트 바인딩
sendBtn.addEventListener("click", sendMessage);
endBtn.addEventListener("click", endSession);
input.addEventListener("keydown", (e) => {
  if (e.key === "Enter" && !e.shiftKey) {
    e.preventDefault();
    sendMessage();
  }
});
