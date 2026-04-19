import json
from datetime import datetime
from pathlib import Path

KNOWLEDGE_FILE = Path(__file__).parent / "data" / "knowledge.json"


def load_knowledge() -> list[dict]:
    if not KNOWLEDGE_FILE.exists():
        return []
    return json.loads(KNOWLEDGE_FILE.read_text(encoding="utf-8")).get("facts", [])


def save_knowledge(facts: list[dict]):
    KNOWLEDGE_FILE.write_text(
        json.dumps({"facts": facts}, ensure_ascii=False, indent=2),
        encoding="utf-8",
    )


def _extract_keywords(text: str) -> list[str]:
    stop = {"은", "는", "이", "가", "을", "를", "의", "에", "도", "와", "과", "로", "으로", "에서", "한다", "하다", "있다"}
    words = text.lower().replace(",", " ").replace(".", " ").split()
    return [w for w in words if len(w) > 1 and w not in stop]


def add_facts(new_facts: list[str]) -> int:
    """새 사실을 추가하고 저장. 중복 제거 후 추가된 수 반환."""
    existing = load_knowledge()
    existing_contents = {f["content"] for f in existing}

    added = 0
    next_id = max((f["id"] for f in existing), default=0) + 1

    for content in new_facts:
        if content in existing_contents:
            continue
        existing.append({
            "id": next_id,
            "content": content,
            "keywords": _extract_keywords(content),
            "created_at": datetime.now().isoformat(timespec="seconds"),
        })
        existing_contents.add(content)
        next_id += 1
        added += 1

    save_knowledge(existing)
    return added


def search_knowledge(query: str, n: int = 5) -> list[str]:
    """키워드 교집합으로 관련 사실 검색."""
    facts = load_knowledge()
    if not facts:
        return []

    query_keywords = set(_extract_keywords(query))
    scored = []
    for fact in facts:
        overlap = query_keywords & set(fact.get("keywords", []))
        if overlap:
            scored.append((len(overlap), fact["content"]))

    scored.sort(reverse=True)
    return [content for _, content in scored[:n]]
