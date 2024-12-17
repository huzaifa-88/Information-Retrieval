import re
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity

# Step 1: Speech-to-Text Conversion
def speech_to_text():
    print("Simulating Speech-to-Text Conversion...")
    return "Find me articles about the benefits of exercise for health"

# Step 2: Text Preprocessing
def preprocess_text(text):
    print("Preprocessing Text...")
    text = text.lower()
    text = re.sub(r'[^a-zA-Z0-9\s]', '', text)
    return text

# Step 3: Semantic Understanding (Keyword Extraction)
def extract_keywords(text):
    print("Extracting Keywords...")
    # For simplicity, consider splitting by spaces as a naive keyword extraction
    keywords = text.split()
    return keywords

# Step 4: Query Expansion
synonyms = {
    "benefits": ["advantages", "gains"],
    "exercise": ["workouts", "physical activity"],
    "health": ["wellness", "fitness"]
}

def expand_query(keywords):
    print("Expanding Query...")
    expanded = []
    for word in keywords:
        expanded.append(word)
        if word in synonyms:
            expanded.extend(synonyms[word])
    return list(set(expanded))

# Step 5 & 6: Search and Retrieval
def search_articles(query):
    print("Searching for Articles...")
    # Sample article dataset
    articles = [
        "Regular exercise has numerous benefits for overall health and wellness.",
        "Studies show that physical activity can improve mental fitness and well-being.",
        "The advantages of workouts include better sleep and a healthier lifestyle.",
        "Exercise routines are vital for maintaining fitness and avoiding diseases."
    ]

    # Vectorizing articles and query
    vectorizer = TfidfVectorizer()
    tfidf_matrix = vectorizer.fit_transform(articles)
    query_vector = vectorizer.transform([" ".join(query)])

    # Calculating similarity
    similarities = cosine_similarity(query_vector, tfidf_matrix)
    ranked_articles = [articles[i] for i in similarities.argsort()[0][::-1]]
    return ranked_articles

# Step 7: Voice Assistant Response
def respond_with_articles(articles):
    print("\nVoice Assistant Response:")
    if articles:
        for idx, article in enumerate(articles[:3], start=1):
            print(f"{idx}. {article}")
    else:
        print("No relevant articles found.")

# Main CLI Function
def main():
    print("Welcome to the Voice Assistant Simulation!")

    # Step 1
    spoken_query = speech_to_text()
    print(f"You said: {spoken_query}\n")

    # Step 2
    processed_query = preprocess_text(spoken_query)

    # Step 3
    keywords = extract_keywords(processed_query)
    print(f"Extracted Keywords: {keywords}\n")

    # Step 4
    expanded_query = expand_query(keywords)
    print(f"Expanded Query: {expanded_query}\n")

    # Step 5 & 6
    articles = search_articles(expanded_query)

    # Step 7
    respond_with_articles(articles)

if __name__ == "__main__":
    main()