import re
import os
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.metrics.pairwise import cosine_similarity
from google.colab import files

# Step 1: Query Input
def input_query():
    query = input("Please enter your query (e.g., 'Find me articles about...'): ")
    return query

# Step 2: Text Preprocessing
def preprocess_text(text):
    text = text.lower()
    text = re.sub(r'[^a-zA-Z0-9\s]', '', text)
    return text

# Step 3: Extract Keywords (without NLTK)
def extract_keywords(text):
    # Tokenize and remove stopwords
    stopwords = set([
        'a', 'an', 'the', 'is', 'it', 'and', 'to', 'of', 'in', 'for', 'on', 'by', 'with', 'at', 'from', 'as', 'that', 'which', 'can', 'be', 'are'
    ])

    words = text.split()
    keywords = [word for word in words if word not in stopwords]
    return keywords

# Step 4: Query Expansion Using Synonyms (with WordNet)
from nltk.corpus import wordnet

def get_synonyms(word):
    synonyms = set()
    for syn in wordnet.synsets(word):
        for lemma in syn.lemmas():
            synonyms.add(lemma.name().replace('_', ' '))
    return list(synonyms)

def expand_query(keywords):
    expanded = []
    for word in keywords:
        expanded.append(word)
        expanded.extend(get_synonyms(word))
    return list(set(expanded))

# Step 5 & 6: Search and Retrieve Relevant Articles
def search_articles(query, folder_path):
    if not os.path.exists(folder_path):
        print(f"Error: The folder '{folder_path}' does not exist.")
        return []

    articles = []
    filenames = []
    for filename in os.listdir(folder_path):
        if filename.endswith(".txt"):
            with open(os.path.join(folder_path, filename), 'r', encoding='utf-8') as file:
                content = file.read()
                articles.append(content)
                filenames.append(filename)

    if not articles:
        print("No articles found in the folder.")
        return []

    vectorizer = TfidfVectorizer()
    tfidf_matrix = vectorizer.fit_transform(articles)
    query_vector = vectorizer.transform([" ".join(query)])

    similarities = cosine_similarity(query_vector, tfidf_matrix)
    ranked_indices = similarities.argsort()[0][::-1]

    top_articles = [(filenames[i], articles[i]) for i in ranked_indices[:3] if similarities[0, i] > 0]
    return top_articles

# Step 7: Display Relevant Articles
def respond_with_articles(articles):
    if articles:
        print("\nRelevant Articles:")
        for idx, (filename, content) in enumerate(articles, start=1):
            print(f"{idx}. File: {filename}\n   Content Preview: {content[:200]}...\n")
    else:
        print("No relevant articles found.")

# Main CLI Function
def main():
    print("Welcome to the Query-Based Article Retrieval System!")
    spoken_query = input_query()
    print(f"You said: {spoken_query}\n")

    processed_query = preprocess_text(spoken_query)

    keywords = extract_keywords(processed_query)
    print(f"Extracted Keywords: {keywords}\n")

    expanded_query = expand_query(keywords)
    print(f"Expanded Query with Synonyms: {expanded_query}\n")

    print("Please upload your .txt files containing articles.")
    uploaded = files.upload()

    folder_path = "/content/articles"
    os.makedirs(folder_path, exist_ok=True)
    for filename in uploaded.keys():
        with open(os.path.join(folder_path, filename), "wb") as f:
            f.write(uploaded[filename])

    articles = search_articles(expanded_query, folder_path)
    respond_with_articles(articles)

if __name__ == "__main__":
    import nltk
    try:
        nltk.download('wordnet', quiet=True, force=True)
    except Exception as e:
        print(f"Error downloading NLTK resources: {e}")
    main()
