import tkinter as tk
from tkinter import ttk, messagebox


class EducationalApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Educational Content Browser")
        self.root.geometry("800x600")

        self.create_frames()

        self.content_data = {
            "Science": {
                "Physics": ["Mechanics", "Thermodynamics"],
                "Chemistry": ["Organic Chemistry", "Inorganic Chemistry"]
            },
            "Mathematics": {
                "Algebra": ["Linear Equations", "Quadratic Equations"],
                "Calculus": ["Limits", "Integration"]
            }
        }

        self.content_details = {
            "Physics": "Physics is a natural science that deals with the physical properties and behavior of matter and energy. Learn more about <Mechanics> and <Thermodynamics>.",
            "Chemistry": "Chemistry is a fundamental science that deals with the composition, structure, properties, and changes of matter. Learn more about <Organic Chemistry> and <Inorganic Chemistry>.",
            "Algebra": "Algebra is a branch of mathematics that deals with the study of linear equations and linear functions. Learn more about <Linear Equations> and <Quadratic Equations>.",
            "Calculus": "Calculus is a branch of mathematics that deals with rates of change and accumulation. Learn more about <Limits> and <Integration>.",
            "Linear Equations": "Linear equations are mathematical expressions for straight lines.",
            "Quadratic Equations": "Quadratic equations have a degree of two and a parabolic graph.",
            "Limits": "Limits explore the behavior of functions as inputs approach a value.",
            "Integration": "Integration is a fundamental calculus operation for areas under curves.",
            "Mechanics": "Mechanics is the branch of physics dealing with motion and forces.",
            "Thermodynamics": "Thermodynamics is the study of heat, work, and energy systems.",
            "Organic Chemistry": "Organic Chemistry focuses on carbon-based compounds.",
            "Inorganic Chemistry": "Inorganic Chemistry covers non-carbon compounds like metals."
        }

        self.create_tree_view()
        self.create_content_display()

    def create_frames(self):
        self.tree_frame = tk.Frame(self.root, width=200, bg="lightgray")
        self.tree_frame.pack(side=tk.LEFT, fill=tk.Y)

        self.content_frame = tk.Frame(self.root, bg="white")
        self.content_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)

    def create_tree_view(self):
        self.tree = ttk.Treeview(self.tree_frame)
        self.tree.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        for subject, topics in self.content_data.items():
            subject_id = self.tree.insert("", "end", text=subject, open=True)
            for topic, subtopics in topics.items():
                topic_id = self.tree.insert(subject_id, "end", text=topic, open=False)
                for subtopic in subtopics:
                    self.tree.insert(topic_id, "end", text=subtopic)

        self.tree.bind("<<TreeviewSelect>>", self.display_content)

    def create_content_display(self):
        self.header_label = tk.Label(
            self.content_frame, text="Select Content", font=("Arial", 20), bg="white", anchor="w"
        )
        self.header_label.pack(fill=tk.X, padx=10, pady=10)

        self.text_area = tk.Text(self.content_frame, wrap=tk.WORD, font=("Arial", 12), state=tk.DISABLED)
        self.text_area.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

    def display_content(self, event):
        selected_item = self.tree.focus()
        selected_text = self.tree.item(selected_item, "text")

        self.header_label.config(text=selected_text)
        self.text_area.config(state=tk.NORMAL)
        self.text_area.delete(1.0, tk.END)

        content = self.content_details.get(selected_text, "No specific content is available for this selection.")
        self.insert_hyperlinks(content)
        self.text_area.config(state=tk.DISABLED)

    def insert_hyperlinks(self, content):
        """Insert text with clickable hyperlinks into the text area."""
        self.text_area.tag_configure("hyperlink", foreground="blue", underline=True)
        self.text_area.tag_bind("hyperlink", "<Button-1>", self.on_hyperlink_click)

        idx = 0
        while idx < len(content):
            if content[idx] == "<":
                end_idx = content.find(">", idx)
                if end_idx != -1:
                    hyperlink_text = content[idx + 1:end_idx]
                    self.text_area.insert(tk.END, hyperlink_text, ("hyperlink", hyperlink_text))
                    idx = end_idx + 1
                    continue
            self.text_area.insert(tk.END, content[idx])
            idx += 1

    def on_hyperlink_click(self, event):
        """Handle clicks on hyperlinks."""
        index = self.text_area.index(f"@{event.x},{event.y}")
        tags = self.text_area.tag_names(index)
        for tag in tags:
            if tag != "hyperlink":
                self.display_content_by_topic(tag)

    def display_content_by_topic(self, topic):
        """Display content for the given topic."""
        if topic in self.content_details:
            self.header_label.config(text=topic)
            self.text_area.config(state=tk.NORMAL)
            self.text_area.delete(1.0, tk.END)
            self.insert_hyperlinks(self.content_details[topic])
            self.text_area.config(state=tk.DISABLED)
        else:
            messagebox.showinfo("Error", f"No content found for {topic}!")


if __name__ == "__main__":
    root = tk.Tk()
    app = EducationalApp(root)
    root.mainloop()











# import tkinter as tk
# from tkinter import ttk, messagebox

# class EducationalApp:
#     def __init__(self, root):
#         self.root = root
#         self.root.title("Educational Content Browser")
#         self.root.geometry("800x600")
    
#         self.create_frames()
        
#         self.content_data = {
#             "Science": {
#                 "Physics": ["Mechanics", "Thermodynamics"],
#                 "Chemistry": ["Organic Chemistry", "Inorganic Chemistry"]
#             },
#             "Mathematics": {
#                 "Algebra": ["Linear Equations", "Quadratic Equations"],
#                 "Calculus": ["Limits", "Integration"]
#             }
#         }
#         self.content_details = {
#             "Physics": "Physics is a natural science that deals with the physical properties and behavior of matter and energy.",
#             "Chemistry": "Chemistry is a fundamental science that deals with the composition, structure, properties, and changes of matter.",
#             "Algebra": "Algebra is a branch of mathematics that deals with the study of linear equations and linear functions.",
#             "Calculus": "Calculus is a branch of mathematics that deals with rates of change and accumulation.",
#             "Linear Equations": "Linear equations are mathematical expressions for straight lines.",
#             "Quadratic Equations": "Quadratic equations have a degree of two and a parabolic graph.",
#             "Limits": "Limits explore the behavior of functions as inputs approach a value.",
#             "Integration": "Integration is a fundamental calculus operation for areas under curves.",
#             "Mechanics": "Mechanics is the branch of physics dealing with motion and forces.",
#             "Thermodynamics": "Thermodynamics is the study of heat, work, and energy systems.",
#             "Organic Chemistry": "Organic Chemistry focuses on carbon-based compounds.",
#             "Inorganic Chemistry": "Inorganic Chemistry covers non-carbon compounds like metals."
#         }
        
#         self.create_tree_view()
#         self.create_content_display()
#         self.create_search_bar()

#     def create_frames(self):
#         self.top_frame = tk.Frame(self.root, bg="lightblue")
#         self.top_frame.pack(side=tk.TOP, fill=tk.X)

#         self.tree_frame = tk.Frame(self.root, width=200, bg="lightgray")
#         self.tree_frame.pack(side=tk.LEFT, fill=tk.Y)

#         self.content_frame = tk.Frame(self.root, bg="white")
#         self.content_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)

#     def create_tree_view(self):
#         self.tree = ttk.Treeview(self.tree_frame)
#         self.tree.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

#         for subject, topics in self.content_data.items():
#             subject_id = self.tree.insert("", "end", text=subject, open=True)
#             for topic, subtopics in topics.items():
#                 topic_id = self.tree.insert(subject_id, "end", text=topic, open=False)
#                 for subtopic in subtopics:
#                     self.tree.insert(topic_id, "end", text=subtopic)

#         self.tree.bind("<<TreeviewSelect>>", self.display_content)

#     def create_content_display(self):
#         self.header_label = tk.Label(
#             self.content_frame, text="Select Content", font=("Arial", 20), bg="white", anchor="w"
#         )
#         self.header_label.pack(fill=tk.X, padx=10, pady=10)
        
#         self.text_area = tk.Text(self.content_frame, wrap=tk.WORD, font=("Arial", 12))
#         self.text_area.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

#         # Related Content Label
#         self.related_label = tk.Label(
#             self.content_frame, text="Related Topics:", font=("Arial", 16), bg="white", anchor="w"
#         )
#         self.related_label.pack(fill=tk.X, padx=10, pady=10)
        
#         self.related_links = tk.Frame(self.content_frame, bg="white")
#         self.related_links.pack(fill=tk.X, padx=10, pady=5)

#     def create_search_bar(self):
#         self.search_entry = tk.Entry(self.top_frame, font=("Arial", 14))
#         self.search_entry.pack(side=tk.LEFT, padx=10, pady=10, fill=tk.X, expand=True)
#         self.search_button = tk.Button(
#             self.top_frame, text="Search", font=("Arial", 14), command=self.search_content
#         )
#         self.search_button.pack(side=tk.LEFT, padx=10, pady=10)

#     def display_content(self, event):
#         selected_item = self.tree.focus()
#         selected_text = self.tree.item(selected_item, "text")
        
#         self.header_label.config(text=selected_text)
#         self.text_area.delete(1.0, tk.END)
        
#         content = self.content_details.get(
#             selected_text, 
#             f"No specific content is available for {selected_text}."
#         )
#         self.text_area.insert(tk.END, content)
        
#         self.display_related_content(selected_text)

#     def display_related_content(self, selected_text):
#         # Clear previous links
#         for widget in self.related_links.winfo_children():
#             widget.destroy()
        
#         related_topics = {
#             "Science": ["Physics", "Chemistry"],
#             "Physics": ["Mechanics", "Thermodynamics"],
#             "Chemistry": ["Organic Chemistry", "Inorganic Chemistry"],
#             "Mathematics": ["Algebra", "Calculus"],
#             "Algebra": ["Linear Equations", "Quadratic Equations"],
#             "Calculus": ["Limits", "Integration"],
#         }
#         links = related_topics.get(selected_text, ["No related topics"])
        
#         for topic in links:
#             link = tk.Label(self.related_links, text=topic, fg="blue", cursor="hand2", bg="white")
#             link.pack(side=tk.LEFT, padx=5)
#             link.bind("<Button-1>", lambda e, t=topic: self.handle_related_topic(t))

#     def handle_related_topic(self, topic):
#         messagebox.showinfo("Related Topic", f"Navigate to {topic}!")

#     def search_content(self):
#         query = self.search_entry.get().strip()
        
#         if not query:
#             messagebox.showwarning("Search", "Please enter a search term.")
#             return

#         result = None
#         for topic, details in self.content_details.items():
#             if query.lower() in topic.lower() or query.lower() in details.lower():
#                 result = topic
#                 break

#         if result:
#             self.header_label.config(text=result)
#             self.text_area.delete(1.0, tk.END)
#             self.text_area.insert(tk.END, self.content_details[result])
#         else:
#             messagebox.showinfo("Search", "No content found matching your search term.")

# if __name__ == "__main__":
#     root = tk.Tk()
#     app = EducationalApp(root)
#     root.mainloop()
