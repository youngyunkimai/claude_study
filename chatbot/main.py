import json
import ollama
from fastapi import FastAPI
from fastapi.responses import StreamingResponse, FileResponse
from fastapi.staticfiles import StaticFiles
from pydantic import BaseModel

from knowledge import search_knowledge, load_knowledge, add_facts
from extractor import extract_facts

MODEL = "gemma3:1b"

app = FastAPI()
app.mount("/static", StaticFiles(directory="static"), name="static")


class ChatRequest(BaseModel):
    message: str
    history: list[dict]


class ExtractRequest(BaseModel):
    history: list[dict]


@app.get("/")
def index():
    return FileResponse("static/index.html")


@app.post("/chat")
def chat(req: ChatRequest):
    # 관련 지식 검색
    relevant = search_knowledge(req.message)

    # 시스템 메시지 구성
    system_content = "당신은 친절한 AI 어시스턴트입니다. 한국어로 대답하세요."
    if relevant:
        facts = "\n".join(f"- {f}" for f in relevant)
        system_content += f"\n\n[알고 있는 관련 정보]\n{facts}"

    messages = [{"role": "system", "content": system_content}]

    # 최근 10턴만 유지
    recent_history = req.history[-20:]
    messages.extend(recent_history)
    messages.append({"role": "user", "content": req.message})

    def generate():
        stream = ollama.chat(model=MODEL, messages=messages, stream=True)
        for chunk in stream:
            token = chunk.message.content
            if token:
                yield token

    return StreamingResponse(generate(), media_type="text/plain; charset=utf-8")


@app.post("/extract")
def extract(req: ExtractRequest):
    facts = extract_facts(req.history)
    added = add_facts(facts) if facts else 0
    return {"extracted": facts, "added": added}


@app.get("/knowledge")
def knowledge():
    return {"facts": load_knowledge()}
