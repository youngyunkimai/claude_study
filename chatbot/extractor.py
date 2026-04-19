import json
import ollama

MODEL = "gemma3:1b"

SYSTEM_PROMPT = """당신은 대화에서 핵심 사실을 추출하는 도우미입니다.
주어진 대화에서 나중에 참고할 만한 사실만 JSON 배열로 반환하세요.
추출할 사실이 없으면 빈 배열 []을 반환하세요.
JSON 배열 외에 다른 텍스트는 절대 포함하지 마세요.

예시 출력:
["사용자의 이름은 김철수이다", "사용자는 Python을 5년째 사용한다"]"""


def extract_facts(history: list[dict]) -> list[str]:
    """대화 히스토리에서 핵심 사실을 추출."""
    if not history:
        return []

    conversation = "\n".join(
        f"{'User' if m['role'] == 'user' else 'Bot'}: {m['content']}"
        for m in history
    )

    try:
        response = ollama.chat(
            model=MODEL,
            messages=[
                {"role": "system", "content": SYSTEM_PROMPT},
                {"role": "user", "content": f"다음 대화에서 핵심 사실을 추출해주세요:\n\n{conversation}"},
            ],
        )
        raw = response.message.content.strip()

        # JSON 배열 파싱
        start = raw.find("[")
        end = raw.rfind("]") + 1
        if start == -1 or end == 0:
            return []
        return json.loads(raw[start:end])
    except Exception:
        return []
