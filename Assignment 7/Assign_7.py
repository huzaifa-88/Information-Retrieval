import os
from collections import Counter

def load_documents_from_folder(folder_path):
    documents = []
    for filename in os.listdir(folder_path):
        file_path = os.path.join(folder_path, filename)
        if os.path.isfile(file_path) and file_path.endswith('.txt'):
            with open(file_path, 'r', encoding='utf-8') as file:
                content = file.read()
                documents.append(content)
    return documents

# -------------------------- Inference Model -------------------------- #
# Function to calculate relevance using the Inference Model
def calculate_relevance_inference_model(query, document, doc_word_freq, query_word_freq, smoothing=0.0001):
    query_words = set(query.lower().split())
    doc_words = set(document.lower().split()) 

    prob_doc = sum(doc_word_freq.get(word, 0) for word in doc_words) / (len(doc_words) + smoothing)
    
    prob_query = sum(query_word_freq.get(word, 0) for word in query_words) / (len(query_words) + smoothing)
    
    common_words = query_words.intersection(doc_words)
    prob_query_given_doc = len(common_words) / (len(query_words) + smoothing)
    
    relevance = (prob_query_given_doc * prob_doc) / (prob_query + smoothing)
    return relevance

# Function to rank documents using the Inference Model
def rank_documents_inference_model(query, documents):
    all_doc_words = [word for doc in documents for word in doc.lower().split()]
    doc_word_freq = Counter(all_doc_words)

    query_words = query.lower().split()
    query_word_freq = Counter(query_words)

    doc_probabilities = []
    for doc_id, doc in enumerate(documents):
        prob = calculate_relevance_inference_model(query, doc, doc_word_freq, query_word_freq)
        doc_probabilities.append((doc_id, prob))

    ranked_docs = sorted(doc_probabilities, key=lambda x: x[1], reverse=True)

    return ranked_docs

# -------------------------- Belief Network -------------------------- #
# Function to calculate relevance using the Belief Network
def calculate_relevance_belief_network(query, document):
    query_words = set(query.lower().split())
    doc_words = set(document.lower().split())

    prob_doc = 1 / len(doc_words) if len(doc_words) > 0 else 0.0001

    common_words = query_words.intersection(doc_words)
    prob_query_given_doc = len(common_words) / len(query_words) if len(query_words) > 0 else 0.0001

    relevance = prob_query_given_doc * prob_doc
    return relevance

# Function to rank documents using the Belief Network
def rank_documents_belief_network(query, documents):
    doc_probabilities = []
    for doc_id, doc in enumerate(documents):
        prob = calculate_relevance_belief_network(query, doc)
        doc_probabilities.append((doc_id, prob))
    ranked_docs = sorted(doc_probabilities, key=lambda x: x[1], reverse=True)
    return ranked_docs

# -------------------------- Automatic Model Selection -------------------------- #
# Function to determine the best model based on query characteristics and initi\al relevance
def select_best_model(query, documents):
    if len(query.split()) <= 3:
        return "Belief Network"

    inference_ranking = rank_documents_inference_model(query, documents)
    belief_ranking = rank_documents_belief_network(query, documents)

    if inference_ranking[0][1] > belief_ranking[0][1]:
        return "Inference Model"
    else:
        return "Belief Network"

# -------------------------- Main Program -------------------------- #
def main():
    folder_path = './'

    if not os.path.isdir(folder_path):
        print("The folder path is invalid. Please check the path and try again.")
        return

    documents = load_documents_from_folder(folder_path)
    print(f"\nLoaded {len(documents)} documents from the folder.")

    num_queries = int(input("\nEnter the number of queries: "))

    queries = []
    for q_id in range(num_queries):
        queries.append(input(f"\nEnter query {q_id + 1}: "))

    for query_id, query in enumerate(queries):
        print(f"\nRanking documents for query '{query}':")

        best_model = select_best_model(query, documents)
        print(f"Selected Model: {best_model}")

        if best_model == "Inference Model":
            ranked_docs = rank_documents_inference_model(query, documents)
        elif best_model == "Belief Network":
            ranked_docs = rank_documents_belief_network(query, documents)
        for doc_id, prob in ranked_docs:
            print(f"Document {doc_id + 1}: Relevance Probability = {prob:.4f}")

if __name__ == "__main__":
    main()